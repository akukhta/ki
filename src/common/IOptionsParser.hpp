#pragma once
#include <string>

class IOptionsParser
{
public:
    virtual std::string getSrc() const = 0;

    virtual std::string getDst() const = 0;
    
    virtual ~IOptionsParser() = default;
};