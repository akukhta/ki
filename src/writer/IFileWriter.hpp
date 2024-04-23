#pragma once
#include <vector>

/// Writes data to the file\n
/// Template instance for writer that receives buffer as function argument
/// \tparam ChunkType Data type represents buffer
template <class ChunkType>
class IFileWriter
{
public:
    virtual void write(ChunkType) = 0;
    virtual void create() = 0;
    virtual void finishWrite() = 0;
    virtual bool isWriteFinished() = 0;

    virtual ~IFileWriter() = default;
};

// Full Template Specialization for writer that does not receive buffer as an argument
template<>
class IFileWriter<void>
{
public:
    virtual void write() = 0;
    virtual void create() = 0;
    virtual void finishWrite() = 0;
    virtual bool isWriteFinished() = 0;

    virtual ~IFileWriter() = default;
};