#define BOOST_TEST_MODULE Test1
#include "../../../../src/common/ParallelCopyTool.hpp"
#include "../../../../src/common/PseudoToolFactory.hpp"
#include "../../../../src/reader/MMapFileReader.hpp"
#include "../../../../src/writer/MMapFileWriter.hpp"
#include "../../../Utiles/TmpFile.hpp"
#include "../../../Utiles/MockedOptionsParser.hpp"
#include <boost/test/included/unit_test.hpp>
#include <chrono>
#include <fstream>
#include <stdexcept>
#include <iostream>

BOOST_AUTO_TEST_CASE(PlainTest)
{
    auto srcFile = TmpFile::create();
    auto dstFile = TmpFile(TmpFile::getRandomName());
    MockedOptionsParser parser(srcFile.fileName, dstFile.fileName);
    
    try
    {
        ToolFactory factory(parser, ToolType::PARALLEL);
        auto cp = factory.createTool();
        cp->copy();
    }
    catch (std::runtime_error const &err)
    {
        std::cerr << err.what() << std::endl;
        BOOST_ASSERT(false);
    }

    auto writenFileSize = std::filesystem::file_size(dstFile.fileName);
    std::vector<unsigned char> resultWritten(writenFileSize);
    std::ifstream file(dstFile.fileName, std::ios_base::binary);
        
    file.read(reinterpret_cast<char*>(resultWritten.data()), writenFileSize);

    BOOST_ASSERT(srcFile.content == resultWritten);
}
