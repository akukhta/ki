#pragma once
#include <vector>
#include <optional>

/// Queue interface
/// \tparam ChunkType type of buffer that queue holds
template <class ChunkType>
class IQueue
{
public:
    virtual void push(ChunkType) = 0;
    virtual std::optional<ChunkType> pop() = 0;
    [[nodiscard]] virtual bool isEmpty() const = 0;

    virtual void close() = 0;
    virtual void open() = 0;

    virtual ~IQueue() = default;
};