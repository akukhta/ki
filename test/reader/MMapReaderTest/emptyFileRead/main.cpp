#define BOOST_TEST_MODULE Test1
#include "../../../../src/reader/MMapFileReader.hpp"
#include "../../../Utiles/TmpFile.hpp"
#include <boost/test/included/unit_test.hpp>
#include <chrono>
#include <fstream>
#include <stdexcept>
#include <iostream>

void f()
{
    auto tmpFile = TmpFile::createEmpty();
    MMapFileReader<std::vector<unsigned char>> reader(tmpFile.fileName);
    reader.open();
}

BOOST_AUTO_TEST_CASE(emptryFileRead)
{
    BOOST_CHECK_EXCEPTION(f(), std::runtime_error, 
        [](std::runtime_error const &e) { return std::string_view(e.what()) 
                == 
                std::string_view("Reader: Can`t create a memory mapping of input file");}
    );
}