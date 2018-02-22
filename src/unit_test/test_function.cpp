#include <lang_utils/function.h>

#define BOOST_TEST_MODULE test_function
#include <boost/test/unit_test.hpp>

using namespace lang_utils;

BOOST_AUTO_TEST_CASE(test_construct) {
    auto obj_constructor = construct<std::pair<int, char>>();

    auto one = obj_constructor(1, 'a');

    BOOST_REQUIRE_EQUAL(one.first, 1);
    BOOST_REQUIRE_EQUAL(one.second, 'a');

    auto two = obj_constructor(4, 'q');
    BOOST_REQUIRE_EQUAL(one.first, 1);
    BOOST_REQUIRE_EQUAL(one.second, 'a');
    BOOST_REQUIRE_EQUAL(two.first, 4);
    BOOST_REQUIRE_EQUAL(two.second, 'q');

    auto ptr_constructor = construct<std::pair<double, char>*>();

    auto three = ptr_constructor(0.5, 'd');
    BOOST_REQUIRE_EQUAL(three->first, 0.5);
    BOOST_REQUIRE_EQUAL(three->second, 'd');

    auto four = ptr_constructor(1.5, 'l');
    BOOST_REQUIRE_EQUAL(three->first, 0.5);
    BOOST_REQUIRE_EQUAL(three->second, 'd');
    BOOST_REQUIRE_EQUAL(four->first, 1.5);
    BOOST_REQUIRE_EQUAL(four->second, 'l');

    delete three;
    BOOST_REQUIRE_EQUAL(four->first, 1.5);
    BOOST_REQUIRE_EQUAL(four->second, 'l');

    delete four;
}
