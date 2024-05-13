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
            ("shobj", value(&sharedObjName)->default_value(""), "Name of shared object");
            ("server", value(&isServer)->default_value(true), "True if server, otherwise client");


        positional_options_description positionals;
        positionals.add("src", 1);
        positionals.add("dst", 1);
        positionals.add("type", 1);
        positionals.add("shobj", 1);
        positionals.add("server", 1);

        variables_map vm;

        store (command_line_parser (argc, argv)
            .positional(positionals)
            .options(desc).run (), vm);

        notify (vm);

        if (vm.count ("help") || ((!vm.count ("src") || !vm.count("dst")) && type != "ipc") || (vm.count("type") && !vm.count("shobj")))
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
        else {
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

    ~OptionsParser() override = default;
private:
    options_description desc{"Kopieren Instrument"}; /// Objects that stores and parses options that could be passed to the process
    
    std::string src; /// Source file (copy-from) absolute path
    std::string dst; /// Destination file (copy-to) absolute path
    std::string type; /// Type of tool to use, i.e in what mode perform the copying (buffered, buffered with optimized file IO, IPC)
    std::string sharedObjName; /// name of shared object, applicable for IPC tool only
    bool isServer;
};