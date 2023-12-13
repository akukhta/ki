#pragma once
#include <vector>

class IFileReader
{
public:
    virtual void open() = 0;
    virtual std::vector<unsigned char> read() = 0;
    virtual bool isReadFinished() = 0;
    virtual void finishRead() = 0;
    
    virtual ~IFileReader() {}
};
