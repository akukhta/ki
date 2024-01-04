#pragma once
#include <vector>

template <class ChunkType>
class IFileWriter
{
public:
    virtual void write(ChunkType) = 0;
    virtual void create() = 0;
    virtual void finishWrite() = 0;
    virtual bool isWriteFinished() = 0;

    virtual ~IFileWriter() {}
};