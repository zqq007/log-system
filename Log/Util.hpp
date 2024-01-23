/*实现工具类的使用
    1、获取时间
    2、判断文件是否存在
    3、获取文件当前路径
    4、创建目录
*/
#ifndef __UTIL__
#define __UTIL__

#include <iostream>
#include <ctime>
#include <sys/stat.h>
#include <string>

namespace Log
{
    class Date
    {
    public:
        static size_t now()
        {
            return (size_t)time(nullptr);
        }
    };

    class File
    {
    public:
        static bool exists(const std::string &pathname)
        {
            struct stat st;
            return (stat(pathname.c_str(), &st) == 0);
        }

        static std::string getFilePath(const std::string &pathname)
        {
            size_t pos = pathname.find_last_of("/\\");
            if (pos == std::string::npos)
                return ".";
            std::string path = pathname.substr(0, pos + 1);
            return path;
        }

        static void createdirectory(const std::string &pathname)
        {
            size_t pos = 0, idx = 0;
            while (idx < pathname.size())
            {
                pos = pathname.find_first_of("/\\", idx);
                if (pos == std::string::npos)
                {
                    mkdir(pathname.c_str(), 0777);
                    break;
                }
                std::string path = pathname.substr(0, pos + 1);
                if (exists(path))
                {
                    idx = pos + 1;
                    continue;
                }
                mkdir(path.c_str(), 0777);
                idx = pos + 1;
            }
        }
    };
}

#endif