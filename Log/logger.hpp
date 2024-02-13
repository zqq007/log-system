#pragma once

#include "util.hpp"
#include "message.hpp"
#include "sink.hpp"
#include "level.hpp"
#include "format.hpp"
#include "looper.hpp"
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
        Logger(const std::string &Logger_name, Loglevel::value &limit_lv, const Formatter::ptr &formatter, std::vector<LogSink::ptr> sinks)
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
            message msg(lv, line, filename, str, Logger_name_);

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
        SyncLogger(const std::string &Logger_name, Loglevel::value &limit_lv, const Formatter::ptr &formatter, std::vector<LogSink::ptr> sinks)
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

    class AsyncLogger : public Logger
    {
    public:
        AsyncLogger(const std::string &Logger_name,
                    Loglevel::value &limit_lv,
                    const Formatter::ptr &formatter,
                    std::vector<LogSink::ptr> sinks,
                    const AsyncType &looper_type)
            : Logger(Logger_name, limit_lv, formatter, sinks),
              looper_(std::make_shared<AsyncLooper>(std::bind(&AsyncLogger::realLog, this, std::placeholders::_1), looper_type))
        {
        }

        /*将数据写入缓冲区*/
        void log(const char *data, size_t length)
        {
            looper_->push(data, length);
        }

        /*实际落地方式*/
        void realLog(Buffer &buf)
        {
            for (auto &sink : sinks_)
            {
                sink->log(buf.begin(), buf.readAblesize());
            }
        }

    private:
        AsyncLooper::ptr looper_;
    };

    /*
    由于创建一个日志器需要先把这个日志器所需要的参数先创建出来，再传参创建日志器对象，比较麻烦，于是，采用建造者模式
    */
    enum struct LoggerType
    {
        LOGGER_SYNC,
        LOGGER_ASYNC
    };

    class LoggerBuilder
    {
    public:
        LoggerBuilder() : logger_type_(LoggerType::LOGGER_SYNC), limit_lv_(Loglevel::value::DEBUG), looper_type_(AsyncType::ASYNC_SAFE)
        {
        }
        void buildLoggerType(LoggerType logger_type) { logger_type_ = logger_type; }
        void buildEnableUnSafeAsync() { looper_type_ = AsyncType::ASYNC_UNSAFE; }
        void buildLoggerName(const std::string &Logger_name) { Logger_name_ = Logger_name; }
        void buildLoggerLevel(Loglevel::value limit_lv) { limit_lv_ = limit_lv; }
        void buildFormatter(const std::string &pattern)
        {
            formatter_ = std::make_shared<Formatter>(pattern);
        }
        template <typename sinktype, typename... Args>
        void buildSinks(Args &&...args)
        {
            LogSink::ptr sinks = SinkFactory::create<sinktype>(std::forward<Args>(args)...);
            sinks_.push_back(sinks);
        }

        virtual Logger::ptr build() = 0;

    protected:
        AsyncType looper_type_;
        LoggerType logger_type_;
        std::string Logger_name_;
        std::vector<LogSink::ptr> sinks_;
        Loglevel::value limit_lv_;
        Formatter::ptr formatter_;
    };

    class LocalLoggerBuilder : public LoggerBuilder
    {
    public:
        Logger::ptr build() override
        {
            assert(!Logger_name_.empty()); // 日志器名字必须要有
            if (!formatter_)               // 如果格式化对象指针为空
            {
                formatter_ = std::make_shared<Formatter>();
            }
            if (sinks_.empty())
            {
                LogSink::ptr sinks = SinkFactory::create<StdoutLogSink>();
                sinks_.push_back(sinks);
            }
            if (logger_type_ == LoggerType::LOGGER_ASYNC)
            {
                return std::make_shared<AsyncLogger>(Logger_name_, limit_lv_, formatter_, sinks_, looper_type_);
            }
            return std::make_shared<SyncLogger>(Logger_name_, limit_lv_, formatter_, sinks_);
        }
    };
}