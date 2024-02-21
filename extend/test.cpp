#include "../log/util.hpp"
#include "../log/level.hpp"
#include "../log/message.hpp"
#include "../log/format.hpp"
#include "../log/sink.hpp"
#include "../log/logger.hpp"
#include "../log/buffer.hpp"
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

void test_log()
{
    Log::Logger::ptr logger = Log::LoggerManager::getinstance().getLogger("asynclogger");

    logger->debug(__FILE__, __LINE__, "%s", "测试...");
    logger->info(__FILE__, __LINE__, "%s", "测试...");
    logger->warn(__FILE__, __LINE__, "%s", "测试...");
    logger->error(__FILE__, __LINE__, "%s", "测试...");
    logger->fatal(__FILE__, __LINE__, "%s", "测试...");

    // size_t cnt = 0;
    // while (cnt < 500000)
    // {
    //     // std::stringstream res;
    //     // res << __FILE__ << __LINE__;
    //     // res << "测试-" << cnt;
    //     logger->fatal(__FILE__, __LINE__, "测试-%d", cnt++);
    //     // cur_size += res.str().size();
    // }
}

int main()
{
    std::unique_ptr<Log::LoggerBuilder> builder(new Log::GlobalLoggerBuilder());
    builder->buildLoggerType(Log::LoggerType::LOGGER_ASYNC);
    builder->buildLoggerName("asynclogger");
    builder->buildLoggerLevel(Log::Loglevel::value::WARN);
    builder->buildFormatter("[%c]%m%n");
    // builder->buildEnableUnSafeAsync();
    builder->buildSinks<RollByTimeSink>("./logfile/roll-by-time-", TimeGap::GAP_SECOND);
    Log::Logger::ptr logger = builder->build();

    // test_log();

    size_t cur = Log::Date::now();
    while (Log::Date::now() < cur + 5)
    {
        logger->fatal(__FILE__, __LINE__, "this is test log");
        // usleep(10000);
        sleep(1);
    }

    return 0;
}