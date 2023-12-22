#pragma once
#include "IFileWriter.hpp"
#include <string>
#include <atomic>

class MMapFileWriter : public IFileWriter
{
public:
    explicit MMapFileWriter(std::string fileName, size_t fileSize);

    virtual void write(std::vector<unsigned char>) override;
    virtual void create() override;
    virtual void finishWrite() override;
    virtual bool isWriteFinished() override;
    
    virtual ~MMapFileWriter() {};

private:
    int fileDesc = -1;
    size_t fileSize = 0;

    unsigned char *mmappedFile = nullptr;

    size_t currentOffset = 0;
    
    std::atomic_bool writeFinished{false};

    std::string fileName{""};
};