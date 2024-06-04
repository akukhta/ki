#include "MultiFileWriter.hpp"
#include <filesystem>
#include "../Common/JsonSettingsParser.hpp"

void TCPIP::MultiFileWriter::registerNewFile(unsigned int ID, TCPIP::FileInfo fileInfo, std::string senderIP)
{
    // Client dir format = Client IP - PID
    std::filesystem::path clientDir = senderIP;

    if (!std::filesystem::exists(rootDir / clientDir))
    {
        std::filesystem::create_directory(rootDir / clientDir);
    }

    // Removing remaining descriptors if a client has disconnected before entire file has been sent
    if (pendingFileInfo.find(ID) != pendingFileInfo.end())
    {
        pendingFileInfo.erase(ID);
    }

    if (filesDescs.find(ID) != filesDescs.end())
    {
        filesDescs.erase(ID);
    }

    // Create ofstream
    filesDescs.insert({ID, std::fopen((rootDir / clientDir / fileInfo.fileName).c_str(), "wb")});

    // Add current file info to the info registry
    pendingFileInfo.insert({ID, std::make_tuple(std::move(fileInfo), 0z, std::move(senderIP))});
}

void TCPIP::MultiFileWriter::finishWriteOfFile(unsigned int ID)
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

    applyFileAttributes(ID);

    Logger::log(std::format("Recevied {} from {}", fileInfo(pendingFileInfo[ID]).fileName,
                            senderIp(pendingFileInfo[ID])));

    filesDescs.erase(ID);
    pendingFileInfo.erase(ID);

    fileWriteFinished(ID);
}

TCPIP::MultiFileWriter::MultiFileWriter(MultiFileWriter::queueType queue) : queue(std::move(queue))
{
    rootDir = JsonSettingsParser::getInstance()->getStorageDirectory();

    if (!std::filesystem::exists(rootDir))
    {
        std::filesystem::create_directory(rootDir);
    }
}

void TCPIP::MultiFileWriter::write()
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

        bytesWritten(pendingFileInfo[id]) += buf->getRequestDataSize();

        if (bytesWritten(pendingFileInfo[id]) >= fileInfo(pendingFileInfo[id]).fileSize) {
            finishWriteOfFile(id);
            Logger::log(std::format("Writing of {} finished", fileInfo(pendingFileInfo[id]).fileName));
        }
    }

    buf->reset();
    queue->returnBuffer(std::move(buf.value()));
}

bool TCPIP::MultiFileWriter::checkClientID(unsigned int clientID)
{
    return filesDescs.find(clientID) != filesDescs.end() && pendingFileInfo.find(clientID) != pendingFileInfo.end();
}

void TCPIP::MultiFileWriter::setFileWriteFinished(std::function<void(int)> cb)
{
    fileWriteFinished = std::move(cb);
}

void TCPIP::MultiFileWriter::applyFileAttributes(unsigned int ID)
{
    chmod((rootDir + senderIp(pendingFileInfo[ID]) + fileInfo(pendingFileInfo[ID]).fileName).c_str(),
          fileInfo(pendingFileInfo[ID]).filePermissions);
}

TCPIP::FileInfo& TCPIP::MultiFileWriter::fileInfo(ExtendedFileInfo &fileInfoExt) {
    return std::get<0>(fileInfoExt);
}

size_t& TCPIP::MultiFileWriter::bytesWritten(ExtendedFileInfo &fileInfoExt) {
    return std::get<1>(fileInfoExt);
}

std::string& TCPIP::MultiFileWriter::senderIp(MultiFileWriter::ExtendedFileInfo &fileInfoExt) {
    return std::get<2>(fileInfoExt);
}
