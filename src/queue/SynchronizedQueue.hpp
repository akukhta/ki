#pragma once
#include "IQueue.hpp"
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>

class SynchronizedQueue : public IQueue
{
public:
    SynchronizedQueue() = default;

    SynchronizedQueue(SynchronizedQueue const &) = delete;
    SynchronizedQueue(SynchronizedQueue &&) = delete;
    SynchronizedQueue& operator=(SynchronizedQueue const &) = delete;
    SynchronizedQueue& operator=(SynchronizedQueue &&) = delete;

    virtual void push(std::vector<unsigned char>) override;
    virtual std::optional<std::vector<unsigned char>> pop() override;
    virtual bool isEmpty() override;

    virtual void close() override;
    virtual void open() override;
private:
    std::queue<std::vector<unsigned char>> queue;
    std::mutex qm;
    std::condition_variable cv;
    bool isQueueOpen;
};