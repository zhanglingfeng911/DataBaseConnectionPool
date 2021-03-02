#include "logger.h"
#include <time.h>
#include <iostream>
#include"TimeStamp.h"


 Logger::Logger()
 {
     
 }
// 获取日志的单例
Logger& Logger::GetInstance()
{
    static Logger logger;
    return logger;
}



// 设置日志级别 
void Logger::SetLogLevel(LogLevel level)
{
    m_loglevel = level;
}

// 写日志  [级别信息] time : msg
void Logger::log(std::string msg)
{
    switch (m_loglevel)
    {
    case INFO:
        std::cout << "[INFO]";
        break;
    case ERROR:
        std::cout << "[ERROR]";
        break;
 
    default:
        break;
    }

    // 打印时间和msg
    std::cout << Timestamp::now().toString() << " : " << msg << std::endl;
}