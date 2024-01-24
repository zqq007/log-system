#pragma once

#include "Util.hpp"
#include <memory>
#include <fstream>

namespace Log
{
    class LogSink
    {
    public:
        using ptr = std::shared_ptr<LogSink>;
        LogSink() {}
        virtual void log(const char *data, int length) = 0;
        virtual ~LogSink() {}
    };

    // 采用标准输出的方式
    class StdoutLogSink : public LogSink
    {
    public:
        void log(const char *data, int length) override {}
    };

    // 采用指定文件的方式
    class FileLogSink : public LogSink
    {
    public:
        FileLogSink(const std::string &path) : path_(path) {}
        void log(const char *data, int length) override {}

    private:
        std::string path_;
        std::ofstream ofs_;
    };

    // 采用滚动文件的方式
    class RollBySizeLogSink : public LogSink
    {
    public:
        RollBySizeLogSink(const std::string &basename, const size_t max) : basename_(basename), max_size_(max) {}
        void log(const char *data, int length) override {}

    private:
        void createNewFile();

    private:
        // 创建文件时以基础文件名+时间命名；
        std::string basename_; // 基础文件名
        std::ofstream ofs_;
        size_t max_size_; // 文件最大的存储空间
        size_t cur_size_; // 记录当前文件大小
    };
}