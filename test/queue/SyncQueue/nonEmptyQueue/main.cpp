#define BOOST_TEST_MODULE Test1
#include "../../../../src/queue/SynchronizedQueue.hpp"
#include "../../../Utiles/TmpFile.hpp"
#include <boost/test/included/unit_test.hpp>
#include <chrono>
#include <fstream>
#include <stdexcept>
#include <iostream>


BOOST_AUTO_TEST_CASE(NonEmptyQueue)
{
    SynchronizedQueue q;
    q.push(std::vector<unsigned char>{});

    BOOST_ASSERT(q.isEmpty() == false);
    BOOST_ASSERT(q.pop().has_value() == true);
}
