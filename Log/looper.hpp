/*实现异步线程工作器*/
#pragma once

#include <iostream>
#include <functional>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>
#include "buffer.hpp"

namespace Log
{
    using func = std::function<void(Buffer &)>;

    enum AsyncType
    {
        ASYNC_SAFE,  // 安全状态，如果缓冲区满了就阻塞
        ASYNC_UNSAFE // 非安全状态，不考虑资源耗尽问题
    };

    class AsyncLooper
    {
    public:
        using ptr = std::shared_ptr<AsyncLooper>;

        AsyncLooper(const func &callback, const AsyncType &looper_type = AsyncType::ASYNC_SAFE)
            : stop_(false), thread_(&AsyncLooper::threadEntry, this), callback_(callback), looper_type_(looper_type) {}
        ~AsyncLooper() { stop(); }

        void stop()
        {
            stop_ = true;
            cond_con_.notify_all(); // 唤醒所有线程
            thread_.join();
        }

        void push(const char *data, size_t length)
        {
            std::unique_lock<std::mutex> lock(mutex_);
            if (looper_type_ == AsyncType::ASYNC_SAFE)
                cond_pro_.wait(lock, [&]()
                               { return producer_.writeAblesize() >= length; });
            producer_.push(data, length);
            cond_con_.notify_one();
        }

    private:
        void threadEntry() // 线程入口函数
        {
            while (true)
            {
                {
                    std::unique_lock<std::mutex> lock(mutex_);
                    /*缓冲区没数据，并且工作器停止标志为真则退出*/
                    if (stop_ && producer_.empty())
                        break;
                    cond_con_.wait(lock, [&]()
                                   { return stop_ || !producer_.empty(); });

                    // 确认有数据，则交换
                    consumer_.swap(producer_);
                    // 唤醒消费者线程
                    if (looper_type_ == AsyncType::ASYNC_SAFE)
                        cond_pro_.notify_all();
                }
                callback_(consumer_); // 回调函数处理消费缓冲区
                consumer_.reset();    // 处理完之后重置
            }
        }

    private:
        func callback_;

    private:
        AsyncType looper_type_;
        std::atomic<bool> stop_; // 工作器停止标志
        std::mutex mutex_;
        Buffer producer_; // 生产者缓冲区
        Buffer consumer_; // 消费者缓冲区
        std::condition_variable cond_pro_;
        std::condition_variable cond_con_;
        std::thread thread_;
    };
}