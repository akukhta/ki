#pragma once
#include <utility>

#include "IFileWriter.hpp"
#include "../queue/BufferedQueue.hpp"

class BufferedFileWriter : IFileWriter<void>
{
public:

    explicit BufferedFileWriter(std::string fileName, 
        std::shared_ptr<FixedBufferQueue> queue);

    void write() override;

    void create() override;

    void finishWrite() override;

    bool isWriteFinished() override;
    
    virtual ~BufferedFileWriter();

private:
    std::FILE* fileDesc{};

    std::atomic_bool writeFinished{false};

    std::string fileName;

    std::shared_ptr<FixedBufferQueue> queue;
};