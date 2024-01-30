/*实现一个缓冲区类*/

#include <vector>
#include <iostream>

namespace Log
{
    class Buffer
    {
    public:
        Buffer();

        /*向缓冲区写入数据*/
        void push(const char *data, size_t length);

        /*返回可读数据的起始位置*/
        const char *begin();

        /*返回可读数据长度*/
        size_t data_size();

        /*对写指针向后偏移*/
        void movewriteidx();

        /*重置读写位置,初始化缓冲区*/
        void reset();

        /*对缓冲区进行交换操作*/
        void swap(const Buffer &buff);

        /*判断缓冲区是否为空*/
        bool empty();

    protected:
        /*对读指针向后偏移*/
        void movereadidx();

    private:
        std::vector<char *> buffer_; // 缓冲区
        size_t read_idx_;            // 当前可读的下标
        size_t write_idx_;           // 当前可写的下标
    };
}