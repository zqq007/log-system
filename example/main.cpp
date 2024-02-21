#include "../log/log.h"

void test_log1(const std::string &name)
{
    Log::Logger::ptr logger = Log::LoggerManager::getinstance().getLogger(name);
    Log::INFO("%s", "测试开始");

    logger->debug("%s", "测试...");
    logger->info("%s", "测试...");
    logger->warn("%s", "测试...");
    logger->error("%s", "测试...");
    logger->fatal("%s", "测试...");

    // size_t cnt = 0;
    // while (cnt < 500000)
    // {
    //     // std::stringstream res;
    //     // res << __FILE__ << __LINE__;
    //     // res << "测试-" << cnt;
    //     // logger->fatal(__FILE__, __LINE__, "测试-%d", cnt++);
    //     // cur_size += res.str().size();

    //     logger->fatal("测试日志-%d", cnt++);
    // }

    Log::INFO("%s", "测试完毕");
}

void test_log2()
{
    Log::DEBUG("%s", "测试...");
    Log::INFO("%s", "测试...");
    Log::WARN("%s", "测试...");
    Log::ERROR("%s", "测试...");
    Log::FATAL("%s", "测试...");

    // size_t cnt = 0;
    // while (cnt < 500000)
    // {
    //     // std::stringstream res;
    //     // res << __FILE__ << __LINE__;
    //     // res << "测试-" << cnt;
    //     // logger->fatal(__FILE__, __LINE__, "测试-%d", cnt++);
    //     // cur_size += res.str().size();

    //     Log::FATAL("测试日志-%d", cnt++);
    // }
}

int main()
{
    std::unique_ptr<Log::LoggerBuilder> builder(new Log::GlobalLoggerBuilder());
    builder->buildLoggerType(Log::LoggerType::LOGGER_SYNC);
    builder->buildLoggerName("synclogger");
    builder->buildLoggerLevel(Log::Loglevel::value::DEBUG);
    builder->buildFormatter("[%c][%f:%l]%m%n");
    builder->buildEnableUnSafeAsync();
    builder->buildSinks<Log::StdoutLogSink>();
    builder->buildSinks<Log::FileLogSink>("./logfile/sync.log");
    builder->build();

    test_log1("synclogger");
    // test_log2();
    return 0;
}