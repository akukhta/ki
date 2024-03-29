#pragma once
#include <vector>

template <class ChunkType>
class IFileReader
{
public:
    virtual void open() = 0;
    virtual ChunkType read() = 0;
    [[nodiscard]] virtual bool isReadFinished() const = 0;
    virtual void finishRead() = 0;
    
    virtual ~IFileReader() = default;
};