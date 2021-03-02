#include "CommonConnectionPool.h"
#include "logger.h"

shared_ptr<Connnection> ConnectionPool::getConnection()
{
    unique_lock<mutex> lock(_queueMutex);
    while (_connectionQue.empty())
    {
        //不能用sleep 因为如果这时候有生产者线程生产了新的连接或者有消费线程归还了连接，则此消费者连接的消费者线程代码应该被唤醒
        if (cv_status::timeout == cv.wait_for(lock, chrono::milliseconds(_connectionTimeout)))
        {
            if (_connectionQue.empty())
            {
                LOG_ERR("获取空闲连接超时 获取连接失败!");
                return nullptr;
            }
        }
    }

    /*
    shared_ptr指针析构时，会把connection资源delete掉，相当于调用connection的析构函数，
    connection就被close掉了
    这里需要自定义shared_ptr的释放方式 把connection直接归还到queue中
    */
    shared_ptr<Connnection> sp(_connectionQue.front(),
                               [&](Connnection *pcon) {
                                   //这是在服务器 应用线程中调用的 所以要考虑队列线程安全
                                   unique_lock<mutex> lock(_queueMutex);
                                   pcon->refreshAlivetime(); //刷新一个开始空闲的起始时间
                                   _connectionQue.push(pcon);
                               });
    _connectionQue.pop();
    //谁消费了队列中最后一个conncetion 谁负责通知生产者生产连接
    cv.notify_all();
    // if (_connectionQue.empty())
    // {

    // }

    return sp;
}

//在functional中 绑定这个函数的this指针，
//因为它是成员函数,可以访问当前类的成员变量
void ConnectionPool::produceConnTask()
{
    //生产者线程一直在循环
    for (;;)
    {
        unique_lock<mutex> lock(_queueMutex);
        //队列不空 生产线程 等待
        while (!_connectionQue.empty())
        {
            cv.wait(lock);
        }
        //连接数量没有到达上限 继续创建连接
        if (_connectionCNT < _maxSize)
        {
            Connnection *p = new Connnection();
            p->connect(_ip, _port, _username, _password, _dbname);
            p->refreshAlivetime(); //刷新一个开始空闲的起始时间
            _connectionQue.push(p);
            _connectionCNT++;
        }
        cv.notify_all(); //唤醒消费者线程继续消费
    }
}

//运行在独立的线程 扫描超过MaxIdletime时间的多余的空闲连接，进行多余连接的回收
void ConnectionPool::scannerConnTask()
{
    for (;;)
    {
        //通过sleep模拟定时效果
        this_thread::sleep_for(chrono::seconds(_maxIdleTime));

        //扫描整个队列 释放多余的连接
        unique_lock<mutex> lock(_queueMutex);
        //只有大于初始连接数InitSize 才说明连接池中有多余的连接
        while (_connectionCNT > _initSize)
        {
            Connnection *p = _connectionQue.front();
            if (p->getAlivetime() >= _maxIdleTime * 1000) //_maxIdleTime:ms  getAlivetime（）：s
            {
                _connectionQue.pop();
                _connectionCNT--;
                delete p;//释放连接资源  会调用 ~connection()->mysql_close();

            }
            else
            {
                break;//因为连接从queue后往前插入 队头的都没超过_maxIdeleTIme 后面的更不会超过
            //所以直接break;
            }
            
        }
    }
}

//连接池的构造函数
ConnectionPool::ConnectionPool()
{
    //加载配置项
    if (!loadCOnfigFile())
    {
        return;
    }
    //创建初始数量的连接 启动过程中不用考虑线程安全 此时只有一个线程
    for (int i = 0; i < _initSize; i++)
    {
        Connnection *p = new Connnection();
        p->connect(_ip, _port, _username, _password, _dbname);
        p->refreshAlivetime(); //刷新一个开始空闲的起始时间
        _connectionQue.push(p);
        _connectionCNT++;
    }

    //1.启动一个新的线程 作为连接的生产者线程
    thread produce(bind(&ConnectionPool::produceConnTask, this));
    produce.detach();

    //2.启动一个新的线程 扫描超过MaxIdletime时间的多余的空闲连接，进行多余连接的回收
    thread scanner(bind(&ConnectionPool::scannerConnTask, this));
    scanner.detach();
}
ConnectionPool *ConnectionPool::getConnetcionPool()
{
    static ConnectionPool pool;
    return &pool;
}

//从配置文件中加载配置项
bool ConnectionPool::loadCOnfigFile()
{
    FILE *pf = fopen("mysql.conf", "r");
    if (pf == nullptr)
    {
        LOG_ERR("mysql.conf file is not exist!");
        return false;
    }

    while (!feof(pf))
    {
        char Line[1024] = {0};
        fgets(Line, 1024, pf);
        string str(Line);

        //判断#号的注释
        if (str[0] == '#' || str.empty())
        {
            continue;
        }
        int idx = str.find("=");
        //配置项不合法
        if (idx == -1)
        {
            continue; //读下一行
        }

        //解析配置项
        string key;
        string value;
        key = str.substr(0, idx);
        //去掉key前后的空格
        Trim(str);
        //ip=127.0.0.1\n
        int endidx = str.find('\n', idx);
        value = str.substr(idx + 1, endidx - idx - 1);
        //去掉value前后的空格
        Trim(value);
        m_configMap.insert({key, value});
    }
    // #数据库连接池的配置文件
    // ip=127.0.0.1
    // port=3306
    // username=root
    // password=123456
    // initSize=10
    // maxSize=1024
    // #最大空闲时间 默认单位：s
    // maxIdleTime=60
    // #最大连接超时时间 单位：ms
    // maxConnectionTimeOut=100
    _ip = m_configMap["ip"];
    _port = atoi(m_configMap["port"].c_str());
    _username = m_configMap["username"];
    _password = m_configMap["password"];
    _initSize = atoi(m_configMap["initSize"].c_str());
    _maxSize = atoi(m_configMap["maxSize"].c_str());
    _maxIdleTime = atoi(m_configMap["maxIdleTime"].c_str());
    _connectionTimeout = atoi(m_configMap["maxConnectionTimeOut"].c_str());
    _dbname = m_configMap["dbname"];
    return true;
}

//去掉配置项=前后的空格
void ConnectionPool::Trim(string &str)
{
    //去掉字符串前面多余的空格
    int idx = str.find_first_not_of(' ');
    if (idx != -1)
    {
        str = str.substr(idx, str.size() - idx);
    }

    //去掉字符串后面多余的空格
    idx = str.find_last_not_of(' ');
    if (idx != -1)
    {
        str = str.substr(0, idx + 1);
    }
}

string ConnectionPool::Load(const string &key)
{
    auto it = m_configMap.find(key);
    if (it == m_configMap.end())
    {
        return "";
    }

    return it->second;
}
