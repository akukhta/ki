#pragma once
#include "../../src/common/IOptionsParser.hpp"

class MockedOptionsParser : public IOptionsParser
{
public:
    MockedOptionsParser(std::string const &src, std::string const &dst)
        : src(src), dst(dst) {}

    virtual std::string getSrc() const override
    {
        return src;
    }

    virtual std::string getDst() const override
    {
        return dst;
    }

private:
    std::string src, dst;
};