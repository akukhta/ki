#pragma once
#include <unordered_map>
#include <memory>
#include "IFileWriter.hpp"
#include "../queue/BufferedQueue.hpp"
#include "../TCPIP/FileInfo.hpp"

class MultiFileWriter : public IFileWriter<void>
{
private:
    using queueType = std::shared_ptr<FixedBufferQueue<std::mutex, std::condition_variable, std::unique_lock, std::deque>>;

public:
    MultiFileWriter(queueType queue);

    void registerNewFile(unsigned int ID, TCPIP::FileInfo fileInfo);

private:
    queueType queue;
};

