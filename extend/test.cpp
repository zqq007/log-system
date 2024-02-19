#include "../Log/util.hpp"
#include "../Log/level.hpp"
#include "../Log/message.hpp"
#include "../Log/format.hpp"
#include "../Log/sink.hpp"
#include "../Log/logger.hpp"
#include "../Log/buffer.hpp"
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