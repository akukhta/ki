#pragma once
#include "IFileReader.hpp"
#include <string_view>
#include <unistd.h>
#include <atomic>

class MMapFileReader : public IFileReader
{
public:
    MMapFileReader(std::string_view fileName);

    virtual void open() override;
    virtual std::vector<unsigned char> read() override;
    virtual bool isReadFinished() override;
    virtual void finishRead() override;
    
    size_t getFileSize();

    ~MMapFileReader(){}

    static inline size_t defaultBufferSize = sysconf(_SC_PAGESIZE);

private:
    int fileDesc;
    size_t fileSize = 0;
    unsigned char *mmappedFile;

    size_t currentOffset = 0;
    std::atomic_bool readFinished;

    std::string_view fileName;
};