#include "Util.hpp"
#include "level.hpp"
#include "message.hpp"
#include "format.hpp"
#include "sink.hpp"
#include <unistd.h>

enum struct TimeGap
{
    GAP_SECOND,
    GAP_MINUTE,
    GAP_HOUR,
    GAP_DAY
};

/*
扩展：通过当前时间属于哪个时间段，然后切换到对应时间段文件
*/
class RollByTimeSink : public Log::LogSink
{
public:
    RollByTimeSink(const std::string &basename, TimeGap gap_type)
        : basename_(basename)
    {
        switch (gap_type)
        {
        case TimeGap::GAP_SECOND:
            gap_size_ = 1;
            break;
        case TimeGap::GAP_MINUTE:
            gap_size_ = 60;
            break;
        case TimeGap::GAP_HOUR:
            gap_size_ = 3600;
            break;
        case TimeGap::GAP_DAY:
            gap_size_ = 86400;
            break;
        }

        cur_gap_ = gap_type == TimeGap::GAP_SECOND ? Log::Date::now() : Log::Date::now() % gap_size_;
        std::string filename = createNewFile();
        Log::File::createdirectory(Log::File::getFilePath(filename));
        ofs_.open(filename, std::ios::binary | std::ios::app);
        assert(ofs_.is_open());
    }

    void log(const char *data, int length) override
    {
        if (Log::Date::now() % gap_size_ != cur_gap_)
        {
            ofs_.close();
            std::string filename = createNewFile();
            ofs_.open(filename, std::ios::binary | std::ios::app);
            assert(ofs_.is_open());
            // cur_gap_ = Log::Date::now() % gap_size_;
        }
        ofs_.write(data, length);
        assert(ofs_.good());
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
        // filename << "-";
        filename << ".log";

        return filename.str();
    }

private:
    std::string basename_;
    std::fstream ofs_;
    size_t cur_gap_;
    size_t gap_size_;
};

int main()
{
    // Log::Date::now();

    // std::string path = "./abc/def/a.txt";
    // Log::File::createdirectory(Log::File::getFilePath(path));

    // std::cout << Log::loglevel::toString(Log::loglevel::value::DEBUG) << std::endl;
    // std::cout << Log::loglevel::toString(Log::loglevel::value::INFO) << std::endl;
    // std::cout << Log::loglevel::toString(Log::loglevel::value::WARN) << std::endl;
    // std::cout << Log::loglevel::toString(Log::loglevel::value::ERROR) << std::endl;
    // std::cout << Log::loglevel::toString(Log::loglevel::value::OFF) << std::endl;

    Log::message msg(Log::Loglevel::value::INFO, __LINE__, __FILE__, "格式化功能测试...", "root");
    Log::Formatter fmt;
    // std::cout << fmt.format(msg) << std::endl;

    std::string str = fmt.format(msg);
    // 标准输出
    // Log::StdoutLogSink stdout_sink;
    // stdout_sink.log(log.c_str(), log.size());
    // Log::LogSink::ptr stdout_ptr = Log::SinkFactory<Log::StdoutLogSink>::create();
    // Log::LogSink::ptr stdout_ptr = Log::SinkFactory::create<Log::StdoutLogSink>();
    // stdout_ptr->log(str.c_str(), str.size());

    // 指定文件
    // Log::FileLogSink file_sink("./log/test.log");
    // file_sink.log(log.c_str(), log.size());
    // Log::LogSink::ptr file_ptr = Log::SinkFactory::create<Log::FileLogSink>("./log/test.log");
    // file_ptr->log(str.c_str(), str.size());

    // 滚动文件
    // size_t cur = 0;
    // size_t cnt = 0;
    // Log::LogSink::ptr roll_ptr = Log::SinkFactory::create<Log::RollBySizeLogSink>("./log/test-", 1024 * 1024);
    // while (cur < 1024 * 1024 * 10)
    // {
    //     // std::string res = str + std::to_string(cnt++);
    //     roll_ptr->log(str.c_str(), str.size());
    //     cur += str.size();
    // }

    Log::LogSink::ptr roll = Log::SinkFactory::create<RollByTimeSink>("./log/test-", TimeGap::GAP_SECOND);
    time_t old = Log::Date::now();
    while (Log::Date::now() < old + 5)
    {
        roll->log(str.c_str(), str.size());
        usleep(1000);
    }
    return 0;
}