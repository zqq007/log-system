#pragma once

#include "logger.hpp"

namespace Log
{
    /*提供获取指定日志器的全局接口（避免用户自己操作单例对象）*/
    Log::Logger::ptr getlogger(const std::string &name)
    {
        return Log::LoggerManager::getinstance().getLogger(name);
    }

    Log::Logger::ptr rootlogger()
    {
        return Log::LoggerManager::getinstance().rootLogger();
    }

/*使用宏函数对日志器函数进行代理（代理模式）*/
#define debug(fmt, ...) debug(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define info(fmt, ...) info(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define warn(fmt, ...) warn(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define error(fmt, ...) error(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define fatal(fmt, ...) fatal(__FILE__, __LINE__, fmt, ##__VA_ARGS__)

/*提供宏函数，直接通过默认日志器进行日志的标准输出打印*/
#define DEBUG(fmt, ...) rootlogger()->debug(fmt, ##__VA_ARGS__)
#define INFO(fmt, ...) rootlogger()->info(fmt, ##__VA_ARGS__)
#define WARN(fmt, ...) rootlogger()->warn(fmt, ##__VA_ARGS__)
#define ERROR(fmt, ...) rootlogger()->error(fmt, ##__VA_ARGS__)
#define FATAL(fmt, ...) rootlogger()->fatal(fmt, ##__VA_ARGS__)
}