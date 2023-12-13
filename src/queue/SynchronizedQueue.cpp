#include "SynchronizedQueue.hpp"

std::optional<std::vector<unsigned char>> SynchronizedQueue::pop()
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

void SynchronizedQueue::push(std::vector<unsigned char> buffer)
 {
    {
        std::unique_lock l(qm);
        queue.push(std::move(buffer));
    }
    
    cv.notify_one();
 }

bool SynchronizedQueue::isEmpty()
{
    return !queue.empty();
}

void SynchronizedQueue::close()
{
    std::scoped_lock l(qm);
    isQueueOpen = false;
}

void SynchronizedQueue::open()
{
    std::scoped_lock l(qm);
    isQueueOpen = true;
}