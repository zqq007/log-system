#pragma once

#include "message.hpp"
#include <memory>
#include <cmath>
#include <vector>
#include <cassert>
#include <sstream>
#include <cstdlib>

namespace Log
{
    // 实现一个格式化子项的基类
    class Formatitem
    {
    public:
        using ptr = std::shared_ptr<Formatitem>;
        virtual void format(std::ostream &out, const Log::message &msg) = 0;
    };
    // 实现格式化子项的派生类：消息、等级、时间、文件名、行号、日志器名、线程id

    class MsgFormat : public Formatitem
    {
    public:
        void format(std::ostream &out, const Log::message &msg) override
        {
            out << msg.payload_;
        }
    };

    class LevelFormat : public Formatitem
    {
    public:
        void format(std::ostream &out, const Log::message &msg) override
        {
            out << Log::Loglevel::toString(msg.level_);
        }
    };

    class TimeFormat : public Formatitem
    {
    public:
        TimeFormat(const std::string timeformat = "%H:%M:%S") : timeformat_(timeformat) {}
        void format(std::ostream &out, const Log::message &msg) override
        {
            struct tm t;
            localtime_r(&msg.ctime_, &t);
            char tmp[32];
            strftime(tmp, 31, timeformat_.c_str(), &t);

            out << tmp;
        }

    private:
        std::string timeformat_;
    };

    class FileFormat : public Formatitem
    {
    public:
        void format(std::ostream &out, const Log::message &msg) override
        {
            out << msg.file_;
        }
    };

    class LineFormat : public Formatitem
    {
    public:
        void format(std::ostream &out, const Log::message &msg) override
        {
            out << msg.line_;
        }
    };

    class LoggerFormat : public Formatitem
    {
    public:
        void format(std::ostream &out, const Log::message &msg) override
        {
            out << msg.logger_;
        }
    };

    class ThreadFormat : public Formatitem
    {
    public:
        void format(std::ostream &out, const Log::message &msg) override
        {
            out << msg.tid_;
        }
    };

    class TabFormat : public Formatitem
    {
    public:
        void format(std::ostream &out, const Log::message &msg) override
        {
            out << "\t";
        }
    };

    class NlineFormat : public Formatitem
    {
    public:
        void format(std::ostream &out, const Log::message &msg) override
        {
            out << "\n";
        }
    };

    class OtherFormat : public Formatitem
    {
    public:
        OtherFormat(const std::string &str) : str_(str) {}
        void format(std::ostream &out, const Log::message &msg) override
        {
            out << str_;
        }

    private:
        std::string str_;
    };

    /*
        %d  表示时间
        %T  表示制表符
        %t  表示线程id
        %f  表示文件名
        %l  表示行号
        %m  表示消息有效载荷
        %p  表示消息等级
        %c  表示日志器名
        %n  表示换行
    */
    class Formatter
    {
    public:
        using ptr = std::shared_ptr<Formatter>;
        Formatter(const std::string &pattern = "[%d{%H:%M:%S}][%t][%c][%f:%l][%p]%T%m%n") : pattern_(pattern)
        {
            assert(parsePattern());
        }

        void format(std::ostream &out, Log::message &msg)
        {
            for (auto &i : items_)
                i->format(out, msg);
        }

        std::string format(Log::message &msg)
        {
            std::stringstream ss;
            format(ss, msg);
            return ss.str();
        }

    private:
        bool parsePattern()
        {
            std::string key, val;
            std::vector<std::pair<std::string, std::string>> fmt_order;
            size_t pos = 0;
            while (pos < pattern_.size())
            {
                if (pattern_[pos] != '%')
                {
                    // 说明这是普通字符
                    val.push_back(pattern_[pos++]);
                    continue;
                }
                // 走到这里说明遇到'%'了
                // 还需要判断%后面跟的是什么
                if (pos + 1 < pattern_.size() && pattern_[pos + 1] == '%')
                {
                    val.push_back('%');
                    pos += 2;
                    continue;
                }

                // 接下来先把直接读取到的普通字符串放到vector数组中
                if (!val.empty())
                {
                    fmt_order.push_back(std::make_pair("", val));
                    val.clear();
                }

                // 处理格式化子项
                pos += 1;
                if (pos == pattern_.size())
                {
                    std::cout << "%后面没有对应的格式化字符";
                    return false;
                }
                key = pattern_[pos];
                pos += 1;
                if (pos < pattern_.size() && pattern_[pos] == '{')
                {
                    pos++;
                    while (pos < pattern_.size() && pattern_[pos] != '}')
                    {
                        val.push_back(pattern_[pos++]);
                    }
                    if (pos == pattern_.size())
                    {
                        std::cout << "{}匹配失败";
                        return false;
                    }
                    pos++;//继续向后匹配
                }
                fmt_order.push_back(std::make_pair(key, val));
                key.clear();
                val.clear();
            }

            // for (const auto &kv : fmt_order)
            // {
            //     std::cout << kv.first << " " << kv.second << std::endl;
            // }
            // std::cout << std::endl;

            // 根据解析得到的数据初始化格式子项数组成员
            for (auto &it : fmt_order)
            {
                items_.push_back(createitem(it.first, it.second));
            }
            return true;
        }

        Formatitem::ptr createitem(std::string &key, std::string &value)
        {
            if (key == "d")
                return std::make_shared<TimeFormat>(value);
            else if (key == "T")
                return std::make_shared<TabFormat>();
            else if (key == "t")
                return std::make_shared<ThreadFormat>();
            else if (key == "f")
                return std::make_shared<FileFormat>();
            else if (key == "l")
                return std::make_shared<LineFormat>();
            else if (key == "m")
                return std::make_shared<MsgFormat>();
            else if (key == "p")
                return std::make_shared<LevelFormat>();
            else if (key == "c")
                return std::make_shared<LoggerFormat>();
            else if (key == "n")
                return std::make_shared<NlineFormat>();
            else
            {
                if (!key.empty())
                    std::cout << "没有对应的%" << key << std::endl;
                return std::make_shared<OtherFormat>(value);
            }
            // abort();
            // return Formatitem::ptr();
        }

    private:
        std::string pattern_;
        std::vector<Formatitem::ptr> items_;
    };
}
