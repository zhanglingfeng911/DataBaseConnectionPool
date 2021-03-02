/*
实现Mysql数据库增删改查操作


*/
#pragma once
#include <mysql/mysql.h>
#include <string>
#include <ctime>
// #include<muduo/base/Logging.h>

using namespace std;
class Connnection
{
private:
    MYSQL *_conn;       //和Msyql_server的连接
    clock_t _alivetime; //记录空闲状态后的起始存活时间

public:
    // 初始化数据库连接
    Connnection();
    // 释放数据库连接资源
    ~Connnection();

    // 连接数据库
    bool connect(string ip, unsigned short port, string user, string password,
                 string dbname);

    // 更新操作 insert、delete、update
    bool update(string sql);

    // 查询操作 select
    MYSQL_RES *query(string sql);

    //刷新一下连接的起始的空闲时间点
    void refreshAlivetime() { _alivetime = clock(); }
    //返回存活的时间
    clock_t getAlivetime() const { return clock() - _alivetime; }
};
