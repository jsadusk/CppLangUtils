#include <lang_utils/iterator.h>

#define BOOST_TEST_MODULE test_iterator
#include <boost/test/unit_test.hpp>

#include <vector>
#include <list>

using namespace lang_utils;

BOOST_AUTO_TEST_CASE(test_dynamic_iterator) {
    std::vector<int> one {1, 3, 5};
    std::list<int> two;

    two.push_back(2);
    two.push_back(4);
    two.push_back(6);
    
    auto oneIter = make_dynamic_iterator(one.begin());
    auto twoIter = make_dynamic_iterator(two.begin());

    BOOST_REQUIRE(oneIter == oneIter);
    BOOST_REQUIRE(oneIter != twoIter);

    std::vector<dynamic_iterator<int>> iters;
    iters.emplace_back(one.begin());
    iters.emplace_back(two.begin());

    BOOST_REQUIRE(!(iters[0] == iters[1]));
    BOOST_REQUIRE(iters[0] == dynamic_iterator<int>(one.begin()));
    BOOST_REQUIRE(iters[1] == dynamic_iterator<int>(two.begin()));
    
    BOOST_REQUIRE_EQUAL(*iters[0], 1);
    BOOST_REQUIRE_EQUAL(*iters[1], 2);

    ++iters[0];
    BOOST_REQUIRE_EQUAL(*iters[0], 3);
    BOOST_REQUIRE_EQUAL(*iters[1], 2);

    BOOST_REQUIRE(!(iters[0] == dynamic_iterator<int>(one.begin())));
                          
    ++iters[1];
    BOOST_REQUIRE_EQUAL(*iters[0], 3);
    BOOST_REQUIRE_EQUAL(*iters[1], 4);

    ++iters[0];
    BOOST_REQUIRE_EQUAL(*iters[0], 5);
    BOOST_REQUIRE_EQUAL(*iters[1], 4);

    ++iters[1];
    BOOST_REQUIRE_EQUAL(*iters[0], 5);
    BOOST_REQUIRE_EQUAL(*iters[1], 6);

    ++iters[0];
    BOOST_REQUIRE(iters[0] == dynamic_iterator<int>(one.end()));
    BOOST_REQUIRE_EQUAL(*iters[1], 6);

    ++iters[1];
    BOOST_REQUIRE(iters[1] == dynamic_iterator<int>(two.end()));
}

BOOST_AUTO_TEST_CASE(test_dynamic_collection) {
    std::vector<int> one {1, 3, 5};
    std::list<int> two;

    two.push_back(2);
    two.push_back(4);
    two.push_back(6);

    dynamic_collection<int> dyn;

    dyn.set(one);

    auto iter1 = one.begin();
    for (int i : dyn) {
        BOOST_REQUIRE(i == *iter1);
        ++iter1;
    }

    dyn.set(two);
    auto iter2 = two.begin();
    for (int i : dyn) {
        BOOST_REQUIRE(i == *iter2);
        ++iter2;
    }
}
