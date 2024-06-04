#pragma once
#include <memory>
#include <mutex>
#include <iostream>

class Logger
{
public:
    static void log(std::string const &message)
    {
        static std::shared_ptr<Logger> instance(new Logger{});
        instance->logImplementation(message);
    }
private:
    void logImplementation(std::string const &message)
    {
        std::scoped_lock lk{mutex};
        std::cout << message << std::endl;
    }

    std::mutex mutex;
};