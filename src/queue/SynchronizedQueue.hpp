#pragma once
#include "IQueue.hpp"
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>

template <class ChunkType> 
class SynchronizedQueue : public IQueue<ChunkType>
{
public:
    SynchronizedQueue() = default;

    SynchronizedQueue(SynchronizedQueue const &) = delete;
    SynchronizedQueue(SynchronizedQueue &&) = delete;
    SynchronizedQueue& operator=(SynchronizedQueue const &) = delete;
    SynchronizedQueue& operator=(SynchronizedQueue &&) = delete;

    virtual void push(ChunkType buffer) override
    {
        {
            std::unique_lock l(qm);
            queue.push(std::move(buffer));
        }
    
        cv.notify_one();
    }
    
    virtual std::optional<ChunkType> pop() override
    {
        std::unique_lock l(qm);
        cv.wait(l, [this](){ return queue.empty() == false || isQueueOpen == false;});

        if (isQueueOpen == false && queue.empty())
        {
            return std::nullopt;
        }
        
        auto buffer = std::move(queue.front());
        queue.pop();
        return buffer;
    }
    
    virtual bool isEmpty() const override
    {
        return queue.empty();
    }

    virtual void close() override
    {
        std::scoped_lock l(qm);
        isQueueOpen = false;
    }

    virtual void open() override
    {
        std::scoped_lock l(qm);
        isQueueOpen = true;
    }

private:
    std::queue<ChunkType> queue;
    std::mutex qm;
    std::condition_variable cv;
    bool isQueueOpen = false;
};