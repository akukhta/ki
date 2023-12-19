#pragma once
#include "IFileWriter.hpp"
#include <string>
#include <atomic>

class MMapFileWriter : public IFileWriter
{
public:
    MMapFileWriter(std::string fileName, size_t fileSize);

    virtual void write(std::vector<unsigned char>) override;
    virtual void create() override;
    virtual void finishWrite() override;
    virtual bool isWriteFinished() override;
    
    virtual ~MMapFileWriter() {};

private:
    int fileDesc;
    size_t fileSize;

    unsigned char *mmappedFile;

    size_t currentOffset = 0;
    
    std::atomic_bool writeFinished;

    std::string fileName;
};