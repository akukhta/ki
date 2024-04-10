#pragma once
#include "IPCToolType.hpp"
#include "IPCProcInfo.hpp"
#include <thread>
#include <chrono>
#include <functional>

class SiblingProcessObserver
{
public:
    SiblingProcessObserver(ProcInfo* processInfo, ProcessType processesToObserve,
        std::function<void()> stopwatchCallback);

    void startObserver();

    ~SiblingProcessObserver();

    SiblingProcessObserver(SiblingProcessObserver const&) = delete;
    SiblingProcessObserver& operator=(SiblingProcessObserver const &) = delete;
    SiblingProcessObserver(SiblingProcessObserver && other) = delete;
    SiblingProcessObserver& operator=(SiblingProcessObserver &&) = delete;

private:
    std::function<void()> stopwatchCallback;
    void observeSiblings(std::stop_token token);
    ProcInfo *processInfo;
    std::jthread observerThread;
    bool stopwatchStarted = false;
    ProcessType processesToObserve;
    std::chrono::seconds waitingTime = std::chrono::seconds{10};

    std::chrono::time_point<std::chrono::high_resolution_clock> prevCheck;
    std::chrono::milliseconds stopwatch;
    size_t ProcInfo::* counterMember;
    std::chrono::milliseconds pollInterval = std::chrono::milliseconds{1000};
};