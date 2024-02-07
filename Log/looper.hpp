/*实现异步线程工作器*/
#pragma once

#include <iostream>
#include <functional>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <thread>
#include "buffer.hpp"

namespace Log
{
    using func = std::function<void(Buffer &)>;
    class AsyncLooper
    {
    public:
        using ptr = std::shared_ptr<AsyncLooper>();

        AsyncLooper();

        void stop();

        void push(const char *data, size_t length);

    private:
        void threadEntry(); // 线程入口函数

    private:
        func callback_;

    private:
        bool stop_; // 工作器停止标志
        std::mutex mutex_;
        Buffer producer_; // 生产者缓冲区
        Buffer consumer_; // 消费者缓冲区
        std::condition_variable pro_;
        std::condition_variable con_;
        std::thread thread_;
    };
}