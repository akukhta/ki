#include "../../../../src/writer/MMapFileWriter.hpp"
#include "../../../Utiles/TmpFile.hpp"
#define BOOST_TEST_MODULE Test1
#include <boost/test/included/unit_test.hpp>
#include <filesystem>
#include <chrono>
#include <fstream>
#include <stdexcept>
#include <iostream>

void f()
{
    auto tmpFile = TmpFile::createEmpty();
    MMapFileWriter writer(tmpFile.fileName, tmpFile.fileSize);
    writer.create();
}

BOOST_AUTO_TEST_CASE(ZeroSizeFileWrite)
{
    BOOST_CHECK_EXCEPTION(f(), std::runtime_error, 
        [](std::runtime_error const &e) { return std::string_view(e.what()) 
                == 
                std::string_view("Writer: Can`t create a memory mapping of input file"); }
    );
}