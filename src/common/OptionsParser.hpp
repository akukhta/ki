#pragma once
#include "IOptionsParser.hpp"
#include <boost/program_options.hpp>
#include <iostream>

using namespace boost::program_options;

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


        positional_options_description positionals;
        positionals.add("src", 1);
        positionals.add("dst", 1);
        positionals.add("type", 1);
        positionals.add("shobj", 1);

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

    std::string getSrc() const override
    {
        return src;
    } 

    std::string getDst() const override
    {
        return dst;
    }

    ToolType getToolType() const override
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

    std::string getSharedObjName() const override
    {
        return sharedObjName;
    }

    ~OptionsParser() override = default;
private:
    options_description desc{"Kopieren Instrument"};
    
    std::string src;
    std::string dst;
    std::string type;
    std::string sharedObjName;
};