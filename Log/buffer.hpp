/*实现一个缓冲区类*/
#pragma once

#include <vector>
#include <iostream>
#include <cassert>
#include <algorithm>

namespace Log
{
#define DEFAULT_BUFFER_SIZE (1 * 1024 * 1024)
#define THRESHOLD_SIZE (8 * 1024 * 1024)
#define INCREMENT_SIZE (1 * 1024 * 1024)
    class Buffer
    {
    public:
        Buffer() : buffer_(DEFAULT_BUFFER_SIZE), write_idx_(0), read_idx_(0) {}

        /*向缓冲区写入数据*/
        void push(const char *data, size_t length)
        {
            /*如果空间不够直接返回*/
            // if (write_idx_ < length)
            //     return;

            /*空间不够直接扩容，用于极限测试*/
            ensureEnoughSize(length);

            std::copy(data, data + length, &buffer_[write_idx_]);
            movewriteidx(length);
        }

        /*返回可读数据的起始位置*/
        const char *begin()
        {
            return &buffer_[read_idx_];
        }

        /*返回可读数据长度*/
        size_t read_data_size()
        {
            return write_idx_ - read_idx_;
        }

        /*返回可写数据长度*/
        size_t write_data_size()
        {
            /*这个函数仅针对固定缓冲区提供，因为如果采用扩容的方式，那么就不存在缓冲区空间不够的情况*/
            return (buffer_.size() - write_idx_);
        }

        /*对读指针向后偏移*/
        void movereadidx(size_t length)
        {
            assert(length <= read_data_size());
            read_idx_ += length;
        }

        /*重置读写位置,初始化缓冲区*/
        void reset()
        {
            read_idx_ = write_idx_ = 0; /*读写位置直接置0，表示没有数据可读并且所有空间都是空闲的*/
        }

        /*对缓冲区进行交换操作*/
        void swap(Buffer &buff)
        {
            std::swap(buffer_, buff.buffer_);
            std::swap(write_idx_, buff.write_idx_);
            std::swap(read_idx_, buff.read_idx_);
        }

        /*判断缓冲区是否为空*/
        bool empty()
        {
            return write_idx_ == read_idx_;
        }

        /*Debug*/
        // void print()
        // {
        //     for (auto &c : buffer_)
        //     {
        //         std::cout << c << " ";
        //     }
        //     std::cout << std::endl;
        // }

    protected:
        /*进行扩容*/
        void ensureEnoughSize(size_t len)
        {
            if (len > write_data_size())
            {
                size_t new_len = 0;
                if (len < THRESHOLD_SIZE)
                    new_len = buffer_.size() * 2 + len;/*如果在指定大小之内，翻倍增长，+len是防止len过大，扩容之后还是小于len的情况*/
                else
                    new_len = buffer_.size() + INCREMENT_SIZE + len;/*超出指定大小，采用线性增长*/
                buffer_.resize(new_len);
            }
        }

        /*对写指针向后偏移*/
        void movewriteidx(size_t length)
        {
            assert(length + write_idx_ <= buffer_.size());
            write_idx_ += length;
        }

    private:
        std::vector<char> buffer_; // 缓冲区
        size_t read_idx_;          // 当前可读的下标
        size_t write_idx_;         // 当前可写的下标
    };
}