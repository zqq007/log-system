#pragma once

#include "Util.hpp"
#include <memory>
#include <fstream>
#include <cassert>
#include <sstream>

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
        void log(const char *data, int length) override
        {
            std::cout.write(data, length);
        }
    };

    // 采用指定文件的方式
    class FileLogSink : public LogSink
    {
    public:
        FileLogSink(const std::string &path) : path_(path)
        {
            Log::File::createdirectory(Log::File::getFilePath(path_));
            ofs_.open(path_, std::ios::binary | std::ios::app);
            assert(ofs_.is_open());
        }
        void log(const char *data, int length) override
        {
            ofs_.write(data, length);
            assert(ofs_.good());
        }

    private:
        std::string path_;
        std::ofstream ofs_;
    };

    // 采用滚动文件的方式
    class RollBySizeLogSink : public LogSink
    {
    public:
        RollBySizeLogSink(const std::string &basename, const size_t max) : basename_(basename), max_size_(max), cur_size_(0)
        {
            std::string filename = createNewFile();
            Log::File::createdirectory(Log::File::getFilePath(filename));
            ofs_.open(filename, std::ios::binary | std::ios::app);
            assert(ofs_.is_open());
        }
        void log(const char *data, int length) override
        {
            if (cur_size_ + length >= max_size_)
            {
                ofs_.close(); // 记得要关闭文件
                cur_size_ = 0;
                std::string newfilename = createNewFile();
                ofs_.open(newfilename, std::ios::binary | std::ios::app);
                assert(ofs_.is_open());
            }
            ofs_.write(data, length);
            assert(ofs_.good());
            cur_size_ += length;
        }

    private:
        std::string createNewFile()
        {
            time_t t = Log::Date::now();
            struct tm lt;
            localtime_r(&t, &lt);
            std::stringstream filename;
            filename << basename_;
            filename << lt.tm_year + 1900;
            filename << lt.tm_mon + 1;
            filename << lt.tm_mday;
            filename << lt.tm_hour;
            filename << lt.tm_min;
            filename << lt.tm_sec;
            filename << "-" << filename_cnt_++;
            filename << ".log";

            return filename.str();
        }

    private:
        // 创建文件时以基础文件名+时间命名；
        std::string basename_; // 基础文件名
        std::ofstream ofs_;
        size_t max_size_;     // 文件最大的存储空间
        size_t cur_size_;     // 记录当前文件大小
        size_t filename_cnt_; // 防止文件写入过快导致写到同一个文件中
    };

    class SinkFactory
    {
    public:
        template <typename sinktype, typename... Args>
        static LogSink::ptr create(Args &&...args)
        {
            return std::make_shared<sinktype>(std::forward<Args>(args)...);
        }
    };
}