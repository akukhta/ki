#include "MultiFileWriter.hpp"
#include <filesystem>
#include "../common/Logger.hpp"

void MultiFileWriter::registerNewFile(unsigned int ID, TCPIP::FileInfo fileInfo)
{
    // Client dir format = Client IP - PID
    std::filesystem::path clientDir = fileInfo.senderIP + "-" + std::to_string(fileInfo.port);

    if (!std::filesystem::exists(rootDir / clientDir))
    {
        std::filesystem::create_directory(rootDir / clientDir);
    }

    // Create ofstream
    filesDescs.insert({ID, std::fopen((rootDir / clientDir / fileInfo.fileName).c_str(), "wb")});

    // Add current file info to the info registry
    filesInfo.insert({ID, std::move(fileInfo)});
}

void MultiFileWriter::finishWriteOfFile(unsigned int ID)
{
    // Sync file content on the disc & close file desc
    std::unique_lock lk(mutex);

    if (!checkClientID(ID))
    {
        return;
    }

    if (filesDescs[ID])
    {
        fsync(filesDescs[ID]->_fileno);
        std::fclose(filesDescs[ID]);
    }

    filesDescs.erase(ID);
    filesInfo.erase(ID);
}

MultiFileWriter::MultiFileWriter(MultiFileWriter::queueType queue) : queue(std::move(queue))
{
    if (!std::filesystem::exists(rootDir))
    {
        std::filesystem::create_directory(rootDir);
    }
}

void MultiFileWriter::write()
{
    auto buf = queue->getFilledBuffer();

    if (!buf || !checkClientID(buf->owningClientID))
    {
        return;
    }

    {
        std::unique_lock lk(mutex);

        auto &id = buf->owningClientID;
        auto data = buf->getRequestData();
        if (data) {
            fwrite(buf->getRequestData(), buf->bytesUsed, 1, filesDescs[id]);
        }
        else
        {
            auto v = rand();
            v += rand();
        }
        filesInfo[id].bytesWritten += buf->bytesUsed;
        Logger::log("File Chunk write");

        buf->reset();
        queue->returnBuffer(std::move(buf.value()));

        if (filesInfo[id].bytesWritten + buf->bytesUsed >= filesInfo[id].fileSize)
        {
            finishWriteOfFile(id);
            Logger::log("Finished writing of the file");
        }
    }
}

bool MultiFileWriter::checkClientID(unsigned int clientID)
{
    std::unique_lock lk(mutex);
    return filesDescs.find(clientID) != filesDescs.end() && filesInfo.find(clientID) != filesInfo.end();
}
