#include "MultiFileWriter.hpp"
#include <filesystem>

void MultiFileWriter::registerNewFile(unsigned int ID, TCPIP::FileInfo fileInfo)
{
    // Client dir format = Client IP - PID
    std::filesystem::path clientDir = fileInfo.senderIP + "-" + std::to_string(fileInfo.pid);

    if (!std::filesystem::exists(rootDir / clientDir))
    {
        std::filesystem::create_directory(rootDir / clientDir);
    }

    // Create ofstream
    filesDescs.insert({ID, std::ofstream{(rootDir / clientDir / fileInfo.fileName).string()}});

    // Add current file info to the info registry
    filesInfo.insert({ID, std::move(fileInfo)});
}

void MultiFileWriter::finishWrite(unsigned int ID)
{
    // Sync file content on the disc & close file desc
    filesDescs[ID].flush();
    filesDescs[ID].close();

    // Remove associated file info & file desc
    filesDescs.erase(ID);
    filesInfo.erase(ID);
}

MultiFileWriter::MultiFileWriter(MultiFileWriter::queueType queue) : queue(std::move(queue))
{
    ;
}

void MultiFileWriter::startWriting()
{
    writingThread = std::jthread(&MultiFileWriter::write, this);
}

void MultiFileWriter::write()
{
    while (!queue->isReadFinished() || !queue->isEmpty())
    {
        auto buffer = queue->getFilledBuffer();
    }
}
