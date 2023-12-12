#define BOOST_TEST_MODULE Test1
#include "../../src/reader/MMapFileReader.hpp"
#include "../Utiles/TmpFile.hpp"
#include <boost/test/included/unit_test.hpp>
#include <chrono>
#include <fstream>
#include <stdexcept>
#include <iostream>


BOOST_AUTO_TEST_CASE(OnePageRead)
{
    auto tmpFile = TmpFile::create();

	auto onePageBuf = std::vector<unsigned char>(tmpFile.content.data(), tmpFile.content.data() + MMapFileReader::defaultBufferSize);
    
    try
    {
        MMapFileReader reader(tmpFile.fileName);
        reader.open();

        auto b = reader.read();

        BOOST_ASSERT(b == onePageBuf);
    }
    catch(std::runtime_error const& err)
    {
        std::cout << err.what() << std::endl;
        BOOST_ASSERT(false);
    }
}

BOOST_AUTO_TEST_CASE(EntireFileRead)
{
    auto tmpFile = TmpFile::create();
    try
    {
        MMapFileReader reader(tmpFile.fileName);
        reader.open();

        std::vector<unsigned char> fileBuffer;
        fileBuffer.reserve(tmpFile.fileSize);

        while (!reader.isReadFinished())
        {
            auto buf = reader.read();
            std::copy(buf.begin(), buf.end(), std::back_inserter(fileBuffer));
        }

        BOOST_ASSERT(tmpFile.content == fileBuffer);
    }
    catch(std::runtime_error const &err)
    {
        std::cout << err.what() << std::endl;
        BOOST_ASSERT(false);
    }
}