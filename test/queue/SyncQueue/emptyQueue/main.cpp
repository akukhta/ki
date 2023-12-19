#define BOOST_TEST_MODULE Test1
#include "../../../../src/queue/SynchronizedQueue.hpp"
#include "../../../Utiles/TmpFile.hpp"
#include <boost/test/included/unit_test.hpp>
#include <chrono>
#include <fstream>
#include <stdexcept>
#include <iostream>


BOOST_AUTO_TEST_CASE(EmptyQueue)
{
    SynchronizedQueue q;
    
    BOOST_ASSERT(q.isEmpty() == true);
    BOOST_ASSERT(q.pop().has_value() == false);
}
