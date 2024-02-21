/*
    1、实现等级类
    2、实现一个接口，把等级转换成字符串
*/
#pragma once

namespace Log
{
    class Loglevel
    {
    public:
        enum class value
        {
            UNKNOW = 0,
            DEBUG,
            INFO,
            WARN,
            ERROR,
            FATAL,
            OFF
        };

        static const char *toString(Loglevel::value level)
        {
            switch (level)
            {
            case Loglevel::value::DEBUG:
                return "DEBUG";
            case Loglevel::value::INFO:
                return "INFO";
            case Loglevel::value::WARN:
                return "WARN";
            case Loglevel::value::ERROR:
                return "ERROR";
            case Loglevel::value::FATAL:
                return "FATAL";
            case Loglevel::value::OFF:
                return "OFF";
            }
            return "UNKNOW";
        }
    };
}
