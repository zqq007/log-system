/*
实现一个日志消息的类
里面的成员：
    1、日志产生时间
    2、日志等级
    3、源文件名称
    4、源文件行号
    5、有效载荷
    6、日志器
    7、线程id
*/

#ifndef __M__MESSAGE_H__
#define __M__MESSAGE_H__

#include <iostream>
#include <string>
#include <thread>
#include "level.hpp"
#include "Util.hpp"

namespace Log
{
    struct message
    {
        time_t ctime_;               // 日志产生时间
        Log::Loglevel::value level_; // 日志等级
        std::thread::id tid_;         // 线程id
        size_t line_;                // 源文件行号
        std::string file_;           // 源文件名称
        std::string payload_;        // 有效载荷
        std::string logger_;         // 日志器

        message(Log::Loglevel::value level, size_t line, std::string file,
                std::string payload, std::string logger)
            : ctime_(Log::Date::now()), level_(level), tid_(std::this_thread::get_id()),
              line_(line), file_(file), payload_(payload), logger_(logger)
        {
        }
    };
}

#endif