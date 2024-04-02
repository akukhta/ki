#define BOOST_TEST_MODULE Test1
#include "../../../src/common/ParallelCopyTool.hpp"
#include "../../../src/common/PseudoToolFactory.hpp"
#include "../../Utiles/TmpFile.hpp"
#include "../../Utiles/MockedOptionsParser.hpp"
#include <boost/test/included/unit_test.hpp>
#include <chrono>
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <thread>
BOOST_AUTO_TEST_CASE(IPCToolTest)
{
    auto srcFile = TmpFile::create();
    auto dstFile = TmpFile(TmpFile::getRandomName());
    auto shobjName = "ipctooltest01";
    MockedOptionsParser parser(srcFile.fileName, dstFile.fileName, ToolType::IPC, shobjName);

    auto trFunc = [&parser]()
    {
        ToolFactory factory(parser, ToolType::IPC);
        auto cp = factory.createTool();
        cp->copy();
    };

    std::thread t1 = std::thread(trFunc);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::thread t2 = std::thread(trFunc);


    t1.join();
    t2.join();

    auto writenFileSize = std::filesystem::file_size(dstFile.fileName);
    std::vector<unsigned char> resultWritten(writenFileSize);
    std::ifstream file(dstFile.fileName, std::ios_base::binary);

    file.read(reinterpret_cast<char*>(resultWritten.data()), writenFileSize);

    BOOST_ASSERT(srcFile.content == resultWritten);
}
