#pragma once
#include "IOptionsParser.hpp"
#include <boost/program_options.hpp>
#include <iostream>

using namespace boost::program_options;

/// Command Line Options parser implementation
/// does parse and return option passed to cmd
/// i.e. not mocked, but real
class OptionsParser : public IOptionsParser
{
public:
    OptionsParser(int argc, char ** argv)
    {
        desc.add_options ()
            ("help,h", "ki /path/to/srcFile path/to/dstFile")
            ("src", value(&src), "Input file")
            ("dst", value(&dst), "Output file")
            ("type", value(&type)->default_value("vbuf"), "Type of tool")
            ("shobj", value(&sharedObjName)->default_value(""), "Name of shared object")
            ("server", bool_switch(&isServer), "True if server, otherwise client")
            ("files", value<std::vector<std::string>>(&files)->multitoken(), "list of files to be sent")
            ("settings", value(&settings), "path to settings.json");

        variables_map vm;

        store(parse_command_line(argc, argv, desc), vm);
        notify(vm);

        if (vm.count ("help") || ((!vm.count ("src") || !vm.count("dst")) && (type != "ipc" && type != "tcpip")) || (vm.count("type") && !vm.count("shobj")))
        {
            std::cerr << desc << "\n";

            throw std::runtime_error("Too few arguments were passed");
        }
    }

    [[nodiscard]] std::string getSrc() const override
    {
        return src;
    } 

    [[nodiscard]] std::string getDst() const override
    {
        return dst;
    }

    [[nodiscard]] ToolType getToolType() const override
    {

        if (type == "vbuf")
        {
            return BUFFERED_PARALLEL;
        }
        else if (type == "mmap")
        {
            return PARALLEL;
        }
        else if (type == "ipc")
        {
            return IPC;
        }
        else if (type == "tcpip")
        {
            return TCPIPTOOL;
        }
        else
        {
            throw std::runtime_error("incorrect tool type");
        }
    }

    [[nodiscard]] std::string getSharedObjName() const override
    {
        return sharedObjName;
    }

    [[nodiscard]] bool getIsServer() const override
    {
        return isServer;
    }

    [[nodiscard]] std::vector<std::string> getFilesToSend() const override
    {
        return files;
    }

    [[nodiscard]] virtual std::string getSettingsPath() const override
    {
        return settings;
    }


    ~OptionsParser() override = default;
private:
    options_description desc{"Kopieren Instrument"}; /// Objects that stores and parses options that could be passed to the process
    
    std::string src; /// Source file (copy-from) absolute path
    std::string dst; /// Destination file (copy-to) absolute path
    std::string type; /// Type of tool to use, i.e in what mode perform the copying (buffered, buffered with optimized file IO, IPC)
    std::string sharedObjName; /// name of shared object, applicable for IPC tool only
    bool isServer = false;
    std::vector<std::string> files;
    std::string settings;
};