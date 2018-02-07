#include <lang_utils/memory.h>

#define BOOST_TEST_MODULE test_memory
#include <boost/test/unit_test.hpp>

class FlagDestroySet {
public:
    FlagDestroySet(bool &flag) : m_flag(flag) {}
    ~FlagDestroySet() {
        m_flag = !m_flag;
    }
private:
    bool &m_flag;
};

BOOST_AUTO_TEST_CASE(test_untyped_unique_ptr) {
    bool flag = true;

    {
        lang_utils::untyped_unique_ptr ptr(new FlagDestroySet(flag));
        BOOST_REQUIRE(flag);
    }
    BOOST_REQUIRE(!flag);
}

BOOST_AUTO_TEST_CASE(test_untyped_pool) {
    bool one = true;
    bool two = true;
    bool three = true;

    {
        lang_utils::untyped_pool pool;

        FlagDestroySet *oneObj = new FlagDestroySet(one);
        pool.push(oneObj);

        FlagDestroySet twoObj(two);
        pool.push(std::move(twoObj));

        pool.emplace<FlagDestroySet>(three);

        BOOST_REQUIRE(one);
        BOOST_REQUIRE(two);
        BOOST_REQUIRE(three);
    }
    BOOST_REQUIRE(!one);
    BOOST_REQUIRE(!two);
    BOOST_REQUIRE(!three);
}
