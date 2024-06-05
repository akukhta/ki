#pragma once
#include <fstream>
#include <chrono>
#include <sstream>
#include <format>
#include <string_view>

class FileLogger
{
public:
    explicit FileLogger(std::string const &fileName)
        : out(std::ofstream(fileName))
    {
        ;
    }

    explicit FileLogger()
        : FileLogger(getCurrentDate())
    {
        ;
    }

    ~FileLogger()
    {
        out.close();
    }

    template <typename... Args>
    void log(std::format_string<Args...> format, Args&&... args)
    {
        out << std::format(format, std::forward<decltype(args)>(args)...) << std::endl;
    }

private:
    std::ofstream out;

    static std::string getCurrentDate()
    {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);

        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%Y_%m_%d_%H:%M");
        return ss.str();
    }

};