#include "BufferedFileReader.hpp"

void BufferedReader::open()
{
    fileDesc = std::fopen(fileName.c_str(), "rb");

    if (fileDesc == nullptr)
    {
        throw std::runtime_error("Reader: Can`t open a file");
    }

    if (fileSize < currentOffset)
    {
        readFinished.store(false);
    }
}

void BufferedReader::read()
{
    auto buf = queue->getFreeBuffer().value();

    if (currentOffset + BUFFER_SIZE <= fileSize)
    {
        buf.bytesUsed = BUFFER_SIZE;
    }
    else
    {
        buf.bytesUsed = fileSize - currentOffset;
    }

    auto readCount = std::min(currentOffset + BUFFER_SIZE, fileSize);

    if (setvbuf(fileDesc, reinterpret_cast<char*>(buf.data), _IOFBF, BUFFER_SIZE))
    {
        throw std::runtime_error("can`t set buffering mode");
    }

    fgetc(fileDesc);

    currentOffset = readCount;

    if (currentOffset < fileSize)
    {
        fseek(fileDesc, BUFFER_SIZE - 1, SEEK_CUR);
    }
    else
    {
        readFinished.store(true);
    }
}

bool BufferedReader::isReadFinished() const
{
    return readFinished.load();
}

void BufferedReader::finishRead()
{
    if (fileDesc)
    {
        std::fclose(fileDesc);
        fileDesc = nullptr;
    }
}

size_t BufferedReader::getFileSize() const
{
    return fileSize;
}

BufferedReader::~BufferedReader()
{
    finishRead();
}

BufferedReader::BufferedReader(std::string fileName, std::shared_ptr<FixedBufferQueue> queue)
    : fileName(std::move(fileName)), queue(std::move(queue))
{
    fileSize = std::filesystem::file_size(this->fileName);
}
