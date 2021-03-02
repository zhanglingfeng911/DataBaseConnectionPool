#include "connnection.h"
#include<iostream>
#include"logger.h"
using namespace std;
// 初始化数据库连接
Connnection::Connnection()
{
    _conn = mysql_init(nullptr);
}
// 释放数据库连接资源
Connnection::~Connnection()
{
    if (_conn != nullptr)
        mysql_close(_conn);
}
// 连接数据库
bool Connnection::connect(string ip, unsigned short port, string user, string password,
                          string dbname)
{
    MYSQL *p = mysql_real_connect(_conn, ip.c_str(), user.c_str(),
                                  password.c_str(), dbname.c_str(), port, nullptr, 0);
    return p != nullptr;
}
// 更新操作 insert、delete、update
bool Connnection::update(string sql)
{
    if (mysql_query(_conn, sql.c_str()))
    {
        LOG_ERR("更新失败:%s",sql.c_str());
        return false;
    }
    return true;
}
// 查询操作 select
MYSQL_RES *Connnection::query(string sql)
{
    if (mysql_query(_conn, sql.c_str()))
    {
        LOG_ERR("查询失败:%s",sql.c_str());
        return nullptr;
    }
    return mysql_use_result(_conn);
}