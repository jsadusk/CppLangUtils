#include <lang_utils/tuple.h>
#include <tuple>
#include <vector>
#include <sstream>

#define BOOST_TEST_MODULE test_tuple
#include <boost/test/unit_test.hpp>

using MyTup = std::tuple<int, char, bool>;

template <typename T>
struct MakeVec { using type = std::vector<T>; };

using MyTransformedTup =
    typename lang_utils::transform_tuple_type<MakeVec, MyTup>::type;

BOOST_AUTO_TEST_CASE(test_transform_tuple) {
    MyTransformedTup tup;

    std::get<0>(tup).push_back(1);
    std::get<1>(tup).push_back('a');
    std::get<2>(tup).push_back(true);

    BOOST_REQUIRE_EQUAL(std::get<0>(tup).front(), 1);
    BOOST_REQUIRE_EQUAL(std::get<1>(tup).front(), 'a');
    BOOST_REQUIRE_EQUAL(std::get<2>(tup).front(), true);
}

BOOST_AUTO_TEST_CASE(test_tuple_sizes_equal) {
    auto tup1 = std::make_tuple(1, 'a', true);
    auto tup2 = std::make_tuple(5.5, "asdf", 1);

    size_t check = lang_utils::tuple_sizes_equal<decltype(tup1),
                                                   decltype(tup2)>::value;
    BOOST_REQUIRE_EQUAL(check, 3);
}

BOOST_AUTO_TEST_CASE(test_foreach_tuple) {
    std::stringstream ss;

    auto stringer = [&ss](auto a, auto b) {
        ss << a << " and " << b << " then ";
    };

    auto tup1 = std::make_tuple(1, 'a', 4);
    auto tup2 = std::make_tuple(5.5, "asdf", 2);

    lang_utils::foreach_tuple(stringer, tup1, tup2);

    BOOST_REQUIRE_EQUAL(ss.str(), "1 and 5.5 then a and asdf then 4 and 2 then ");
}

struct indexed {
    indexed(std::stringstream &ss) : m_ss(ss) {}
    
    template <size_t POS, typename ARG1, typename ARG2>
    void operator()(ARG1 &&a, ARG2 &&b) {
        m_ss << "on " << POS << " its " << a << " and " << b << " then ";
    }

    std::stringstream &m_ss;
};

BOOST_AUTO_TEST_CASE(test_foreach_tuple_p) {
    std::stringstream ss;

    indexed stringer(ss);

    auto tup1 = std::make_tuple(1, 'a', 4);
    auto tup2 = std::make_tuple(5.5, "asdf", 2);

    lang_utils::foreach_tuple_p(stringer, tup1, tup2);

    BOOST_REQUIRE_EQUAL(ss.str(),
                        "on 0 its 1 and 5.5 then on 1 its a and asdf then on 2 its 4 and 2 then ");
}

BOOST_AUTO_TEST_CASE(test_foreach_tuple_i) {
    std::stringstream ss;

    auto stringer = [&ss](size_t i, auto a, auto b) {
        ss << "on " << i << " its " << a << " and " << b << " then ";
    };

    auto tup1 = std::make_tuple(1, 'a', 4);
    auto tup2 = std::make_tuple(5.5, "asdf", 2);

    lang_utils::foreach_tuple_i(stringer, tup1, tup2);

    BOOST_REQUIRE_EQUAL(ss.str(),
                        "on 0 its 1 and 5.5 then on 1 its a and asdf then on 2 its 4 and 2 then ");
}

BOOST_AUTO_TEST_CASE(test_map_tuple) {
    auto stringer = [](auto a, auto b) {
        std::stringstream ss;
        ss << a << " and " << b;
        return ss.str();
    };

    auto tup1 = std::make_tuple(1, 'a', 4);
    auto tup2 = std::make_tuple(5.5, "asdf", 2);

    auto tup3 = lang_utils::map_tuple(stringer, tup1, tup2);

    BOOST_REQUIRE_EQUAL(std::get<0>(tup3), "1 and 5.5");
    BOOST_REQUIRE_EQUAL(std::get<1>(tup3), "a and asdf");
    BOOST_REQUIRE_EQUAL(std::get<2>(tup3), "4 and 2");
}

BOOST_AUTO_TEST_CASE(test_reduce_tuple) {
    auto stringer = [](const std::string &accum, auto a, auto b) {
        std::stringstream ss;
        ss << a << " and " << b << " then ";
        return accum + ss.str();
    };
    
    auto tup1 = std::make_tuple(1, 'a', 4);
    auto tup2 = std::make_tuple(5.5, "asdf", 2);

    std::string result = lang_utils::reduce_tuple(stringer, std::string(), tup1, tup2);

    BOOST_REQUIRE_EQUAL(result, "1 and 5.5 then a and asdf then 4 and 2 then ");
}
