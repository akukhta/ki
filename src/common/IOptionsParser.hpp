#pragma once
#include <string>
#include "ToolTypeEnum.hpp"

class IOptionsParser
{
public:
    virtual std::string getSrc() const = 0;

    virtual std::string getDst() const = 0;

    virtual ToolType getToolType() const = 0;

    virtual std::string getSharedObjName() const = 0;

    virtual ~IOptionsParser() = default;
};