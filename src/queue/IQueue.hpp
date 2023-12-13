#pragma once
#include <vector>
#include <optional>

class IQueue
{
public:
    virtual void push(std::vector<unsigned char>) = 0;
    virtual std::optional<std::vector<unsigned char>> pop() = 0;
    virtual bool isEmpty() = 0;
    
    // Are those interfaces apliable to queue itself? Should i segregate interfaces? 
    virtual void close() = 0;
    virtual void open() = 0;

    virtual ~IQueue(){}
};