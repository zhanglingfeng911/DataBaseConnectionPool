/*
实现数据库连接池操作


*/
#pragma once
#include <string>
#include <queue>
#include "connnection.h"
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <atomic>
#include <thread>
#include <memory>
#include <functional>
using namespace std;

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
class ConnectionPool
{
private:
    ConnectionPool();
    //运行在独立的线程中，专门负责产生新连接
    void produceConnTask(); //在functional中 绑定这个函数的this指针，因为它是成员函数,可以访问当前类的成员变量

    //运行在独立的线程 扫描超过MaxIdletime时间的多余的空闲连接，进行多余连接的回收
    void scannerConnTask();
    void Trim(string &str);
    string _ip;             //mysql的ip地址
    unsigned short _port;   //msyql的端口号 3306
    string _username;       //mysql登陆用户名
    string _password;       //mysql登陆密码
    string _dbname;         //连接的数据库名称
    int _initSize;          //初始连接队列大小
    int _maxSize;           //最大连接队列大小
    int _maxIdleTime;       //连接在这个时间没有被用过 就要被清理回收掉
    int _connectionTimeout; //在这个时间内 没有获取到连接 则连接失败
    unordered_map<string, string> m_configMap;
    mutex _queueMutex; //维护连接队列的线程安全互斥锁

    //设置条件变量 用于生产者消费者通信
    condition_variable cv;
    bool loadCOnfigFile();
    string Load(const string &key);
    queue<Connnection *> _connectionQue; //获取与mysql数据库连接的队列
    atomic_int _connectionCNT;           //记录所创建的connection连接的总数量 不能超过maxSIze
public:
    //给外部提供接口 从连接池中获取一个可用的空闲连接
    //通过智能指针 这个连接在用户用完之后会自己释放 不用用户自己关闭连接
    shared_ptr<Connnection> getConnection();

    static ConnectionPool *getConnetcionPool(); //获取对象池连接实例
};
