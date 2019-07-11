#define BOOST_TEST_MODULE main test module
#include <boost/test/included/unit_test.hpp>

#include "include.hpp"q

BOOST_AUTO_TEST_CASE( test_function )
/* Compare with void free_test_function() */
{
    BOOST_TEST( true /* test assertion */ );
}