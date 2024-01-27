#include "Util.hpp"
#include "level.hpp"
#include "message.hpp"
#include "format.hpp"
#include "sink.hpp"

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
    Log::LogSink::ptr stdout_ptr = Log::SinkFactory::create<Log::StdoutLogSink>();
    stdout_ptr->log(str.c_str(), str.size());

    // 指定文件
    // Log::FileLogSink file_sink("./log/test.log");
    // file_sink.log(log.c_str(), log.size());
    Log::LogSink::ptr file_ptr = Log::SinkFactory::create<Log::FileLogSink>("./log/test.log");
    file_ptr->log(str.c_str(), str.size());

    // 滚动文件
    size_t cur = 0;
    size_t cnt = 0;
    Log::LogSink::ptr roll_ptr = Log::SinkFactory::create<Log::RollBySizeLogSink>("./log/test-", 1024 * 1024);
    while (cur < 1024 * 1024 * 10)
    {
        // std::string res = str + std::to_string(cnt++);
        roll_ptr->log(str.c_str(), str.size());
        cur += str.size();
    }
    return 0;
}