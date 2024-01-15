#include "BufferedQueue.hpp"

FixedBufferQueue::FixedBufferQueue()
{
    for (auto & buffer : buffers)
    {
        readBuffers.emplace_back(readBuffers, writeBuffers, cv, buffer.data());
    }
}

std::optional<Buffer> FixedBufferQueue::getFreeBuffer()
{
    std::unique_lock lm(queueMutex);
    cv.wait(lm, [this](){ return !readBuffers.empty() || !isOpen.load(); });

    if (readBuffers.empty() && !isOpen.load())
    {
        return std::nullopt;
    }

    auto buffer = std::move(readBuffers.back());
    readBuffers.pop_back();

    buffer.setType(BufferType::READ);

    return buffer;
}

std::optional<Buffer> FixedBufferQueue::getFilledBuffer()
{
    std::unique_lock lm(queueMutex);
    cv.wait(lm, [this](){ return !writeBuffers.empty() || !isOpen.load(); });

    if (writeBuffers.empty() && !isOpen.load())
    {
        return std::nullopt;
    }

    auto buffer = std::move(writeBuffers.front());
    writeBuffers.pop_front();

    buffer.setType(BufferType::WRITE);
    return buffer;
}

bool FixedBufferQueue::isEmpty() const
{
    std::unique_lock lm(queueMutex);
    return writeBuffers.empty() && !isOpen.load();
}

void FixedBufferQueue::close()
{
    isOpen.store(false);
}

void FixedBufferQueue::open()
{
    isOpen.store(true);
}
