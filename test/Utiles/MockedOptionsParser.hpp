#pragma once
#include "../../src/common/IOptionsParser.hpp"

class MockedOptionsParser : public IOptionsParser {
public:
    MockedOptionsParser(std::string const &src, std::string const &dst, ToolType type, std::string const &shobj = "")
            : src(src), dst(dst), shobjName(shobj), toolType(type) {}

    virtual std::string getSrc() const override {
        return src;
    }

    virtual std::string getDst() const override {
        return dst;
    }

    ToolType getToolType() const override
    {
        return ToolType::PARALLEL;
    }

    std::string getSharedObjName() const override
    {
        return {};
    }

private:
    std::string src, dst, shobjName;
    ToolType toolType;
};