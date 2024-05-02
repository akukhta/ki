#pragma once
#include <unordered_map>
#include <memory>
#include "IFileWriter.hpp"
#include "../queue/BufferedQueue.hpp"
#include "../TCPIP/FileInfo.hpp"
#include <fstream>
#include <thread>

class MultiFileWriter
{
private:
    using queueType = std::shared_ptr<FixedBufferQueue<NonIPCTag>>;

public:
    explicit MultiFileWriter(queueType queue);

    void registerNewFile(unsigned int ID, TCPIP::FileInfo fileInfo);
    void finishWrite(unsigned int ID);

    void startWriting();

private:

    void write();

    queueType queue;
    std::unordered_map<unsigned int, TCPIP::FileInfo> filesInfo;
    std::unordered_map<unsigned int, std::ofstream> filesDescs;

    std::string constexpr static rootDir = "TCPIP Storage";

    std::jthread writingThread;
};

