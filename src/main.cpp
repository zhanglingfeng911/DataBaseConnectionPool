#include "connnection.h"
#include <iostream>
#include "CommonConnectionPool.h"

using namespace std;


int main()
{

        // cp->loadCOnfigFile();
        // cout<<cp->Load("ip")<<endl;
        // cout<<cp->Load("port")<<endl;
        // cout<<cp->Load("username")<<endl;

        
        //单线程
        //-----------------------------------------------------------------------
        //1. 不适用连接池
        clock_t begin = clock();
        for (int i = 0; i < 1000; i++)
        {
                Connnection conn;
                char sql[1024] = {0};
                sprintf(sql, "insert into testuser (name,password,state) values ('%s','%s','%s')",
                        "lklj", "123456", "offline");
                conn.connect("127.0.0.1", 3306, "root", "123456", "chat");
                conn.update(sql);
        }
        clock_t end = clock();
        cout << (end - begin) / 1000 << "ms" << endl;


        //2.使用连接池
        // clock_t begin = clock();
        // ConnectionPool *cp = ConnectionPool::getConnetcionPool();
        // for (int i = 0; i < 1000; i++)
        // {
        //         char sql[1024] = {0};
        //         sprintf(sql, "insert into testuser (name,password,state) values ('%s','%s','%s')",
        //                 "lklj", "123456", "offline");
        //         shared_ptr<Connnection>sp=cp->getConnection();
        //         sp->connect("127.0.0.1", 3306, "root", "123456", "chat");
        //         sp->update(sql);
        // }
        // clock_t end = clock();
        // cout << (end - begin) / 1000 << "ms" << endl;
        return 0;
}
int main01()
{

        // cp->loadCOnfigFile();
        // cout<<cp->Load("ip")<<endl;
        // cout<<cp->Load("port")<<endl;
        // cout<<cp->Load("username")<<endl;


        //单线程
        //-----------------------------------------------------------------------
        //1. 不适用连接池
        clock_t begin = clock();
        for (int i = 0; i < 1000; i++)
        {
                Connnection conn;
                char sql[1024] = {0};
                sprintf(sql, "insert into testuser (name,password,state) values ('%s','%s','%s')",
                        "lklj", "123456", "offline");
                conn.connect("127.0.0.1", 3306, "root", "123456", "chat");
                conn.update(sql);
        }
        clock_t end = clock();
        cout << (end - begin) / 1000 << "ms" << endl;


        //2.使用连接池
        // clock_t begin = clock();
        // ConnectionPool *cp = ConnectionPool::getConnetcionPool();
        // for (int i = 0; i < 1000; i++)
        // {
        //         char sql[1024] = {0};
        //         sprintf(sql, "insert into testuser (name,password,state) values ('%s','%s','%s')",
        //                 "lklj", "123456", "offline");
        //         shared_ptr<Connnection>sp=cp->getConnection();
        //         sp->connect("127.0.0.1", 3306, "root", "123456", "chat");
        //         sp->update(sql);
        // }
        // clock_t end = clock();
        // cout << (end - begin) / 1000 << "ms" << endl;
        return 0;
}
