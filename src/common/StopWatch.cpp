#include "StopWatch.h"
#include <stdexcept>
#include <iostream>

StopWatch StopWatch::createAutoStartWatch(std::string_view desc)
{
    return StopWatch(true, desc);
}

StopWatch StopWatch::createManualWatch(std::string_view desc)
{
    return StopWatch(false, desc);
}

void StopWatch::start()
{
    if (!isAutoMode)
    {
        mStart = std::chrono::high_resolution_clock::now();
    }
    else
    {
        throw std::runtime_error("start() and stop() can`t be applied to stopwatch in auto mode");
    }
}

void StopWatch::stop()
{
    if (!isAutoMode)
    {
        mEnd = std::chrono::high_resolution_clock::now();
    }
    else
    {
        throw std::runtime_error("start() and stop() can`t be applied to stopwatch in auto mode");
    }
}

StopWatch::StopWatch(bool mode, std::string_view desc): isAutoMode(mode), desc(desc)
{
    if (isAutoMode)
    {
        mStart = std::chrono::high_resolution_clock::now();
    }
}

StopWatch::~StopWatch()
{
    if (isAutoMode)
    {
        mEnd = std::chrono::high_resolution_clock::now();
        std::cout << "StopWatch: [" << desc << "]: "<< getStamp<std::chrono::milliseconds>() << std::endl;
    }
}
