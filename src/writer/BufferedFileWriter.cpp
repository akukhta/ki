#include "BufferedFileWriter.hpp"

void BufferedFileWriter::create()
{
    fileDesc = std::fopen(fileName.c_str(), "wb");

    if (fileDesc == nullptr)
    {
        throw std::runtime_error("can`t create output file");
    }

    if (setvbuf(fileDesc, nullptr, _IOFBF, BUFFER_SIZE))
    {
        throw std::runtime_error("can`t set buffering mode for output file");
    }
}

void BufferedFileWriter::finishWrite()
{
    if (fileDesc)
    {
        writeFinished.store(true);
        fsync(fileDesc->_fileno);
        std::fclose(fileDesc);
        fileDesc = nullptr;
    }
}

bool BufferedFileWriter::isWriteFinished()
{
    return writeFinished.load();
}

BufferedFileWriter::~BufferedFileWriter()
{
    finishWrite();
}

BufferedFileWriter::BufferedFileWriter(std::string fileName, std::shared_ptr<FixedBufferQueue> queue)
    :   fileName(std::move(fileName)), queue(std::move(queue))
{
}

void BufferedFileWriter::write()
{
    auto buf = queue->getFilledBuffer();

    if (buf) {
        fwrite(buf->data, buf->bytesUsed, 1, fileDesc);
    }
}
