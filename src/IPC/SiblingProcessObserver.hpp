#pragma once
#include "IPCToolType.hpp"
#include "IPCProcInfo.hpp"
#include <thread>
#include <chrono>
#include <functional>

/// Class that observes how many process are running\n
/// Used to stop execution if no reader/writer available\n
/// Non-movable and non-copyable
class SiblingProcessObserver
{
public:
    ///
    /// \param processInfo process info to gather data from
    /// \param processesToObserve what process to observe/track: reader or writer
    /// \param stopwatchCallback function that is being executed after some delay if no "assymerric" process is running
    SiblingProcessObserver(ProcInfo* processInfo, ProcessType processesToObserve,
        std::function<void()> stopwatchCallback);

    /// Function to start the observation\n
    /// Observation is being cancelled during destruction\n
    /// Observation is done in its own thread
    void startObserver();

    /// Supresses the observation thread
    ~SiblingProcessObserver();

    SiblingProcessObserver(SiblingProcessObserver const&) = delete;
    SiblingProcessObserver& operator=(SiblingProcessObserver const &) = delete;
    SiblingProcessObserver(SiblingProcessObserver && other) = delete;
    SiblingProcessObserver& operator=(SiblingProcessObserver &&) = delete;

private:
    std::function<void()> stopwatchCallback;

    /// Thread Function
    void observeSiblings(std::stop_token token);

    ProcInfo *processInfo;

    /// Observation thread
    std::jthread observerThread;

    bool stopwatchStarted = false;
    ProcessType processesToObserve;

    /// Time to wait before shutdown/callback call
    std::chrono::seconds waitingTime = std::chrono::seconds{10};

    std::chrono::time_point<std::chrono::high_resolution_clock> prevCheck;
    std::chrono::milliseconds stopwatch;
    size_t ProcInfo::* counterMember;

    /// Update/poll time interval
    std::chrono::milliseconds pollInterval = std::chrono::milliseconds{1000};
};