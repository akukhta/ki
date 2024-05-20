#pragma once
#include <unordered_map>
#include <memory>
#include "IFileWriter.hpp"
#include "../queue/BufferedQueue.hpp"
#include "../TCPIP/Common/FileInfo.hpp"
#include <fstream>
#include <thread>

class MultiFileWriter
{
private:
    using queueType = std::shared_ptr<FixedBufferQueue<TCPIPTag>>;

public:
    explicit MultiFileWriter(queueType queue);

    void registerNewFile(unsigned int ID, TCPIP::FileInfo fileInfo);
    void finishWriteOfFile(unsigned int ID);

    void write();

private:
    bool checkClientID(unsigned int clientID);

    queueType queue;
    std::unordered_map<unsigned int, TCPIP::FileInfo> filesInfo;
    std::unordered_map<unsigned int, std::FILE*> filesDescs;

    std::string constexpr static rootDir = "TCPIP Storage";

    std::mutex mutex;
};

