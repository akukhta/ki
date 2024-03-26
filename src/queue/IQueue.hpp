#pragma once
#include <vector>
#include <optional>

template <class ChunkType>
class IQueue
{
public:
    virtual void push(ChunkType) = 0;
    virtual std::optional<ChunkType> pop() = 0;
    [[nodiscard]] virtual bool isEmpty() const = 0;
    
    // Are those interfaces apliable to queue itself? Should i segregate interfaces? 
    virtual void close() = 0;
    virtual void open() = 0;

    virtual ~IQueue() = default;
};