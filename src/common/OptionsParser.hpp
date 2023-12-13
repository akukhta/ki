#pragma once
#include <boost/program_options.hpp>
#include <iostream>

using namespace boost::program_options;

class OptionsParser
{
public:
    OptionsParser(int argc, char ** argv)
    {
        desc.add_options ()
            ("help,h", "ki /path/to/srcFile path/to/dstFile")
            ("src", value(&src), "Input file")
            ("dst", value(&dst), "Output file");

        positional_options_description positionals;
        positionals.add("src", 1);
        positionals.add("dst", 1);
        
        variables_map vm;

        store (command_line_parser (argc, argv)
            .positional(positionals)
            .options(desc).run (), vm);

        notify (vm);

        if (vm.count ("help") || !vm.count ("src") || !vm.count("dst")) {
            std::cerr << desc << "\n";
        }
    }

    std::string getSrc()
    {
        return src;
    } 

    std::string getDst()
    {
        return dst;
    }
    
private:
    options_description desc{"Kopieren Instrument"};
    
    std::string src;
    std::string dst;
};