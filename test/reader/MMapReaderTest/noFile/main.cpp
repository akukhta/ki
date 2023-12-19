#define BOOST_TEST_MODULE Test2
#include "../../../../src/reader/MMapFileReader.hpp"
#include "../../../Utiles/TmpFile.hpp"
#include <boost/test/included/unit_test.hpp>
#include <chrono>
#include <fstream>
#include <stdexcept>
#include <iostream>

BOOST_AUTO_TEST_CASE(NoFileToRead)
{
    BOOST_CHECK_EXCEPTION(MMapFileReader("nofile"), std::runtime_error, 
        [](std::runtime_error const &e)
            { return std::string_view(e.what()) == std::string_view("can`t open src file");});
}