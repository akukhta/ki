#include "MultiFileWriter.hpp"
#include <filesystem>
#include "../common/Logger.hpp"
#include "../TCPIP/Request/RequestHeader.hpp"

void MultiFileWriter::registerNewFile(unsigned int ID, TCPIP::FileInfo fileInfo)
{
    // Client dir format = Client IP - PID
    std::filesystem::path clientDir = fileInfo.senderIP;

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
    fileWriteFinished(ID);
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

    {
        std::unique_lock lk(mutex);
        if (!buf || !checkClientID(buf->owningClientID))
        {
            return;
        }
    }

    auto &id = buf->owningClientID;
    auto data = buf->getRequestData();

    {
        std::unique_lock lk(mutex);

        if (data) {
            fwrite(data, buf->getRequestDataSize(), 1, filesDescs[id]);
        }

        filesInfo[id].bytesWritten += buf->getRequestDataSize();

        if (filesInfo[id].bytesWritten >= filesInfo[id].fileSize) {
            finishWriteOfFile(id);
            Logger::log("Finished writing of the file");
        }
    }

    buf->reset();
    queue->returnBuffer(std::move(buf.value()));
}

bool MultiFileWriter::checkClientID(unsigned int clientID)
{
    return filesDescs.find(clientID) != filesDescs.end() && filesInfo.find(clientID) != filesInfo.end();
}

void MultiFileWriter::setFileWriteFinished(std::function<void(int)> cb)
{
    fileWriteFinished = std::move(cb);
}
