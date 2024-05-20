#pragma once
#include <memory>
#include <mutex>
#include <iostream>

class Logger
{
public:
    static std::shared_ptr<Logger> getInstance()
    {
        static std::shared_ptr<Logger> instance(new Logger{});
        return instance;
    }

    static void log(std::string const &message)
    {
        getInstance()->logImplementation(message);
    }
private:

    void logImplementation(std::string const &message)
    {
        std::scoped_lock lk{mutex};
        std::cout << message << std::endl;
    }

    std::mutex mutex;
};