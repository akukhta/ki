#include "SiblingProcessObserver.hpp"
#include "IPCProcInfo.hpp"

SiblingProcessObserver::SiblingProcessObserver(ProcInfo *processInfo, ProcessType processesToObserve,
    std::function<void()> stopwatchCallback)
        : processInfo(processInfo), processesToObserve(processesToObserve), stopwatchCallback(std::move(stopwatchCallback))
{
    if (processesToObserve == ProcessType::WriterProcess)
    {
        counterMember = &ProcInfo::writerProcessCount;
    }
    else if (processesToObserve == ProcessType::ReaderProcess)
    {
        counterMember = &ProcInfo::readerProcessCount;
    }
    else
    {
        throw std::runtime_error("SiblingProcessObserver can`t observe process that run Invalid tool");
    }
}

void SiblingProcessObserver::observeSiblings(std::stop_token token)
{
    while (!token.stop_requested())
    {
        if (!processInfo)
        {
            std::this_thread::sleep_for(pollInterval);
            continue;
        }

        if (auto lock = processInfo->createScopedLock(); (processInfo->*counterMember))
        {
            // Siblings are running
            // Stop the stopwatch
            stopwatchStarted = false;
        }
        else
        {
            if (!stopwatchStarted)
            {
                // Sibling is not running
                // Start the stopwatch
                // Set the flag to true
                // And clear the stopwatch from previous runs
                stopwatchStarted = true;
                prevCheck = std::chrono::high_resolution_clock::now();
                stopwatch = std::chrono::seconds(0);
            }
            else
            {
                // Timer has already been started
                auto now = std::chrono::high_resolution_clock::now();
                stopwatch += std::chrono::duration_cast<std::chrono::milliseconds>(now - prevCheck);
                prevCheck = now;
            }

            // If stopwatch has exceeded the waiting time
            // Call stopwatch callback
            if (std::chrono::duration_cast<std::chrono::seconds>(stopwatch) >= waitingTime)
            {
                // Check if the callback is assigned
                if (stopwatchCallback)
                {
                    stopwatchCallback();
                }

                return;
            }

            std::this_thread::sleep_for(pollInterval);
        }
    }
}

SiblingProcessObserver::~SiblingProcessObserver()
{
    observerThread.request_stop();
}

void SiblingProcessObserver::startObserver()
{
    observerThread = std::jthread(&SiblingProcessObserver::observeSiblings, this);
}
