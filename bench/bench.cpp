#include "../log/log.h"
#include <vector>
#include <thread>
#include <chrono>

void bench(const std::string &loggername, size_t thread_cnt, size_t msg_cnt, size_t msg_len)
{
    /*
        1、获取日志器
        2、组织指定长度的日志消息
        3、创建指定数量的线程
        4、线程函数内部开始计时
        5、开始循环写日志
        6、线程函数内部结束计时
        7、计算总耗时
        8、进行输出打印
    */
    Log::Logger::ptr logger = Log::getlogger(loggername);
    if (logger.get() == nullptr)
        return;

    std::cout << "测试日志:" << msg_cnt << "条,总大小:" << msg_cnt * msg_len / 1024 << "KB" << std::endl;

    std::string msg(msg_len - 1, 'A'); /*少一个字节是为了后面再添加换行符*/

    std::vector<std::thread> threads;
    std::vector<double> cost_array(thread_cnt);
    for (int i = 0; i < thread_cnt; ++i)
    {
        threads.emplace_back([&, i]()
                             {
            auto start = std::chrono::high_resolution_clock::now();

            size_t thr_average_msg = msg_cnt / thread_cnt;
            for(int j = 0; j < thr_average_msg; ++j)
            {
                logger->info("%s",msg.c_str());
            }

            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> cost = end - start;
            cost_array[i] = cost.count();

            std::cout << "\t线程" << i << "\t输出日志数量:" << thr_average_msg << ",耗时时长:" << cost.count() << "s" << std::endl; });
    }

    for (int i = 0; i < thread_cnt; ++i)
        threads[i].join();

    double max_cost = cost_array[0];
    for (auto &i : cost_array)
        max_cost = max_cost < i ? i : max_cost;

    double msg_average_cost = msg_cnt / max_cost;                     // 每秒输出数量
    double size_average_cost = (msg_cnt * msg_len / 1024) / max_cost; // 以KB为单位计算每秒输出多大数据

    std::cout << "\t总耗时:" << max_cost << std::endl;
    std::cout << "\t每秒输出数量:" << msg_average_cost << "条" << std::endl;
    std::cout << "\t每秒输出大小:" << size_average_cost << "KB" << std::endl;
}

void sync_bench()
{
    std::unique_ptr<Log::LoggerBuilder> builder(new Log::GlobalLoggerBuilder());
    builder->buildLoggerType(Log::LoggerType::LOGGER_SYNC);
    builder->buildLoggerName("synclogger");
    builder->buildFormatter("%m%n");
    // builder->buildEnableUnSafeAsync();
    builder->buildSinks<Log::FileLogSink>("./logfile/sync.log");
    builder->build();

    // bench("synclogger", 1, 1000000, 100);
    bench("synclogger", 3, 1000000, 100);
}

void async_bench()
{
    std::unique_ptr<Log::LoggerBuilder> builder(new Log::GlobalLoggerBuilder());
    builder->buildLoggerType(Log::LoggerType::LOGGER_ASYNC);
    builder->buildLoggerName("asynclogger");
    builder->buildFormatter("%m%n");
    builder->buildEnableUnSafeAsync(); // 开启非安全模式
    // builder->buildEnableUnSafeAsync();
    builder->buildSinks<Log::FileLogSink>("./logfile/async.log");
    builder->build();

    // bench("asynclogger", 1, 1000000, 100);
    bench("asynclogger", 3, 1000000, 100);
}

int main()
{
    // sync_bench();
    async_bench();
    return 0;
}