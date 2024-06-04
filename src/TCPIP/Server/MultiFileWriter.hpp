#pragma once
#include <unordered_map>
#include <memory>
#include "../../writer/IFileWriter.hpp"
#include "../../queue/BufferedQueue.hpp"
#include "../Common/FileInfo.hpp"
#include <fstream>
#include <thread>
#include <functional>

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
    void applyFileAttributes(unsigned int ID);

    queueType queue;
    std::unordered_map<unsigned int, TCPIP::FileInfo> filesInfo;
    std::unordered_map<unsigned int, std::FILE*> filesDescs;

    void setFileWriteFinished(std::function<void(int)> cb);
    std::function<void(int)> fileWriteFinished;

    std::mutex mutex;
    std::string rootDir;
    friend class ToolFactory;
};

