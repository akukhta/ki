#include "SynchronizedQueue.hpp"

std::optional<std::vector<unsigned char>> SynchronizedQueue::pop()
{
    std::scoped_lock l(qm);
    
    if (queue.empty())
    {
        return std::nullopt;
    }
    
    auto buffer = std::move(queue.front());
    queue.pop();

    return buffer;
}

void SynchronizedQueue::push(std::vector<unsigned char> buffer)
 {
    std::scoped_lock l(qm);
    queue.push(std::move(buffer));
 }
