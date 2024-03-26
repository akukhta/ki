#pragma once
#include "IQueue.hpp"
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <concepts>

template <class ChunkType>
    requires std::movable<ChunkType>
class SynchronizedQueue : public IQueue<ChunkType>
{
public:
    SynchronizedQueue() = default;

    SynchronizedQueue(SynchronizedQueue const &) = delete;
    SynchronizedQueue(SynchronizedQueue &&) = delete;
    SynchronizedQueue& operator=(SynchronizedQueue const &) = delete;
    SynchronizedQueue& operator=(SynchronizedQueue &&) = delete;

    void push(ChunkType buffer) override
    {
        {
            std::unique_lock l(qm);
            queue.push(std::move(buffer));
        }
    
        cv.notify_one();
    }
    
    std::optional<ChunkType> pop() override
    {
        std::unique_lock l(qm);
        cv.wait(l, [this](){ return queue.empty() == false || !isQueueOpen;});

        if (!isQueueOpen && queue.empty())
        {
            return std::nullopt;
        }
        
        auto buffer = std::move(queue.front());
        queue.pop();
        return buffer;
    }
    
    bool isEmpty() const override
    {
        std::unique_lock lm(qm);
        return queue.empty() && !isQueueOpen;
    }

    void close() override
    {
        std::scoped_lock l(qm);
        isQueueOpen = false;
    }

    void open() override
    {
        std::scoped_lock l(qm);
        isQueueOpen = true;
    }

private:
    std::queue<ChunkType> queue;
    mutable std::mutex qm;
    std::condition_variable cv;
    bool isQueueOpen = false;
};