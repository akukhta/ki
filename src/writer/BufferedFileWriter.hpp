#pragma once
#include "IFileWriter.hpp"
#include "../queue/BufferedQueue.hpp"

class BufferedFileWriter
{
public:
    explicit BufferedFileWriter(std::string fileName, 
        std::shared_ptr<FixedBufferQueue<Buffer>> queue)
        :   fileName(fileName), queue(queue) {}

    virtual void write()
    {
        auto buf = queue->getFilledBuffer();
        fwrite(buf.data, BUFFER_SIZE, 1, fileDesc);
    }

    virtual void create()
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

    virtual void finishWrite()
    {
        writeFinished.store(true);
        std::fclose(fileDesc);
    }

    virtual bool isWriteFinished()
    {
        return writeFinished.load();
    }
    
    virtual ~BufferedFileWriter() {};

private:
    std::FILE* fileDesc;

    std::atomic_bool writeFinished{false};

    std::string fileName{""};

    std::shared_ptr<FixedBufferQueue<Buffer>> queue;
};