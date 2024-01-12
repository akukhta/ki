//
// Created by alex on 12.01.24.
//
#include <chrono>

class StopWatch
{
public:
    static StopWatch createAutoStartWatch(std::string_view desc);
    static StopWatch createManualWatch(std::string_view desc);

    void start();
    void stop();

    template <typename T>
    T getStamp()
    {
        return std::chrono::duration_cast<T>(mEnd - mStart);
    }

    ~StopWatch();

private:

    explicit StopWatch(bool mode, std::string_view desc);

    std::chrono::time_point<std::chrono::system_clock> mStart;
    std::chrono::time_point<std::chrono::system_clock> mEnd;

    bool isAutoMode = false;

    std::string_view desc;
};
