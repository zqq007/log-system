#include "Util.hpp"
#include "level.hpp"
#include "message.hpp"
#include "format.hpp"

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
    Log::Formatter fmt("%g[%d{%H:%M:%S}][%f:%l][%t]%T%m%n");
    std::cout << fmt.format(msg) << std::endl;
    return 0;
}