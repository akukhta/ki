#pragma once
#include <string>
#include <vector>
#include "ToolTypeEnum.hpp"

/// Command Line Options parser interface
/// Implemented via interface to support "Mocked" parsers
/// that are used in unit tests
class IOptionsParser
{
public:
    /// Returns path to the source (copy-from) file, path is expected to be absolute
    virtual std::string getSrc() const = 0;

    /// Returns path to the destination (copy-to) file, path is expected to be absolute
    virtual std::string getDst() const = 0;

    /// Returns the type of tool to use
    virtual ToolType getToolType() const = 0;

    /// Returns the name of shared object is used for IPC
    /// Only usable for IPC tool (--type=IPC), any other tool ignores the option
    virtual std::string getSharedObjName() const = 0;

    /// Returns bool if tcpip tool instance should serve as server
    /// Otherwise false
    virtual bool getIsServer() const = 0;

    virtual std::vector<std::string> getFilesToSend() const = 0;

    virtual std::string getSettingsPath() const = 0;

    virtual ~IOptionsParser() = default;
};