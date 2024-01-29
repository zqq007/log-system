#pragma once

#include "Util.hpp"
#include "message.hpp"
#include "sink.hpp"
#include "level.hpp"
#include "format.hpp"
#include <atomic>
#include <mutex>
#include <vector>
#include <cstdarg>

/*
日志器模块
    1、具体日志器采用不同的日志落地方式
    2、再派生出不同的子类，分别为同步日志&异步日志两个模式
*/

namespace Log
{
    class Logger
    {
    public:
        using ptr = std::shared_ptr<Logger>;
        Logger(const std::string &Logger_name, Loglevel::value &limit_lv, Formatter::ptr formatter, std::vector<LogSink::ptr> sinks)
            : Logger_name_(Logger_name), limit_lv_(limit_lv), formatter_(formatter), sinks_(sinks) {}

        /*
        完成日志消息对象构造过程并进行格式化，得到格式化后的日志消息字符串，再进行实际落地方式
            1、判断当前日志是否达到输出等级
            2、根据给定的参数得出一个日志消息字符串
            3、构造message对象
            4、通过格式化工具对message对象进行格式化，得到格式化后的字符串
            5、进行日志落地
        */
        void debug(const std::string &filename, size_t line, const std::string &fmt, ...)
        {
            if (Loglevel::value::DEBUG < limit_lv_)
                return;
            va_list ap;
            va_start(ap, fmt);
            char *str;
            int ret = vasprintf(&str, fmt.c_str(), ap);
            if (ret == -1)
            {
                std::cout << "vasprintf error\n";
                va_end(ap);
                return;
            }
            Serialization(Loglevel::value::DEBUG, filename, line, str);
            free(str);
        }

        void info(const std::string &filename, size_t line, const std::string &fmt, ...)
        {
            if (Loglevel::value::INFO < limit_lv_)
                return;
            va_list ap;
            va_start(ap, fmt);
            char *str;
            int ret = vasprintf(&str, fmt.c_str(), ap);
            if (ret == -1)
            {
                std::cout << "vasprintf error\n";
                va_end(ap);
                return;
            }
            Serialization(Loglevel::value::INFO, filename, line, str);
            free(str);
        }

        void warn(const std::string &filename, size_t line, const std::string &fmt, ...)
        {
            if (Loglevel::value::WARN < limit_lv_)
                return;
            va_list ap;
            va_start(ap, fmt);
            char *str;
            int ret = vasprintf(&str, fmt.c_str(), ap);
            if (ret == -1)
            {
                std::cout << "vasprintf error\n";
                va_end(ap);
                return;
            }
            Serialization(Loglevel::value::WARN, filename, line, str);
            free(str);
        }

        void error(const std::string &filename, size_t line, const std::string &fmt, ...)
        {
            if (Loglevel::value::ERROR < limit_lv_)
                return;
            va_list ap;
            va_start(ap, fmt);
            char *str;
            int ret = vasprintf(&str, fmt.c_str(), ap);
            if (ret == -1)
            {
                std::cout << "vasprintf error\n";
                va_end(ap);
                return;
            }
            Serialization(Loglevel::value::ERROR, filename, line, str);
            free(str);
        }

        void fatal(const std::string &filename, size_t line, const std::string &fmt, ...)
        {
            if (Loglevel::value::FATAL < limit_lv_)
                return;
            va_list ap;
            va_start(ap, fmt);
            char *str;
            int ret = vasprintf(&str, fmt.c_str(), ap);
            if (ret == -1)
            {
                std::cout << "vasprintf error\n";
                va_end(ap);
                return;
            }
            Serialization(Loglevel::value::FATAL, filename, line, str);
            free(str);
        }

    protected:
        void Serialization(Loglevel::value lv, const std::string &filename, size_t line, char *str)
        {
            message msg(Loglevel::value::DEBUG, line, filename, str, Logger_name_);

            std::string res = formatter_->format(msg);

            log(res.c_str(), res.size());
        }

        virtual void log(const char *data, size_t length) = 0;

    protected:
        std::mutex mutex_;
        std::string Logger_name_;
        std::vector<LogSink::ptr> sinks_;
        std::atomic<Loglevel::value> limit_lv_;
        Formatter::ptr formatter_;
    };

    class SyncLogger : public Logger
    {
    public:
        SyncLogger(const std::string &Logger_name, Loglevel::value &limit_lv, Formatter::ptr formatter, std::vector<LogSink::ptr> sinks)
            : Logger(Logger_name, limit_lv, formatter, sinks)
        {
        }

        void log(const char *data, size_t length) override
        {
            std::unique_lock<std::mutex> lock(mutex_);
            for (auto &sink : sinks_)
            {
                sink->log(data, length);
            }
        }
    };
}