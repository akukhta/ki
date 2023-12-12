#include "../../src/writer/MMapFileWriter.hpp"
#include "../Utiles/TmpFile.hpp"
#define BOOST_TEST_MODULE Test1
#include <boost/test/included/unit_test.hpp>
#include <filesystem>
#include <chrono>
#include <fstream>
#include <stdexcept>
#include <iostream>

BOOST_AUTO_TEST_CASE(EntireFileWrite)
{
    auto tmpFile = TmpFile::create();

    try
    {
        TmpFile resultFileWrapper("result");
        MMapFileWriter writer("result", tmpFile.fileSize);
        writer.create();
        writer.write(tmpFile.content);
        writer.finishWrite(true);


        auto writenFileSize = std::filesystem::file_size("result");
        std::vector<unsigned char> resultWritten(writenFileSize);
        std::ifstream file("result", std::ios_base::binary);
        
        file.read(reinterpret_cast<char*>(resultWritten.data()), writenFileSize);

        BOOST_ASSERT(tmpFile.content == resultWritten);
    }
    catch (std::runtime_error const &err)
    {
        std::cout << err.what() << std::endl;
        BOOST_ASSERT(false);
    }
}