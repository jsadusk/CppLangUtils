#ifndef LANG_UTILS_TUPLE_H
#define LANG_UTILS_TUPLE_H

#include <tuple>
#include <utility>

namespace lang_utils {

template <template <typename> typename TRANSFORM, typename ORIG>
class transform_tuple_type {
private:
    template <typename INDEX_SEQ> struct Impl;

    template <size_t ...INDICES>
    struct Impl<std::index_sequence<INDICES...>> {
        using type =
            std::tuple<
                typename TRANSFORM<
                    typename std::tuple_element<INDICES, ORIG>::type>::type...>;
    };

public:
    using type =
        typename Impl<std::make_index_sequence<
                          std::tuple_size<ORIG>::value>>::type;
};

template <typename ...TUPLES>
struct tuple_sizes_equal;

template <typename TUPLE>
struct tuple_sizes_equal<TUPLE>{
    static const size_t value = std::tuple_size<typename std::decay<TUPLE>::type>::value;
};

template <typename ...TUPLES>
struct tuple_sizes_equal {
    template <typename FIRST, typename ...REMAINING>
    struct helper {
        static const size_t value =
            std::tuple_size<typename std::decay<FIRST>::type>::value;
        static_assert(tuple_sizes_equal<REMAINING...>::value == value,
                      "tuple sizes not equal");
    };

    static const size_t value = helper<TUPLES...>::value;
};

template <typename FUNC, typename ...TUPLES>
class foreach_tuple_helper {
    template <typename F, typename ...T>
    friend void foreach_tuple(F&&, T&&...);

    void operator()(std::index_sequence<>, FUNC &&func,
                    TUPLES&& ...tuples) { }

    template <size_t POS, size_t ...INDICES>
    void operator()(std::index_sequence<POS, INDICES...>, FUNC &&func,
                  TUPLES&& ...tuples) {
        func(std::get<POS>(tuples)...);
        (*this)(std::index_sequence<INDICES...>(),
                std::forward<FUNC>(func),
                std::forward<TUPLES>(tuples)...);
    }
};

template <typename FUNC, typename ...TUPLES>
void foreach_tuple(FUNC &&func, TUPLES&& ...tuples) {
    static const size_t size = tuple_sizes_equal<TUPLES...>::value;

    foreach_tuple_helper<FUNC, TUPLES...>()(std::make_index_sequence<size>(),
                                            std::forward<FUNC>(func),
                                            std::forward<TUPLES>(tuples)...);
}

template <typename FUNC, typename ...TUPLES>
class foreach_tuple_i_helper {
    template <typename F, typename ...T>
    friend void foreach_tuple_i(F&&, T&&...);
    
    void operator()(std::index_sequence<>, FUNC &&func,
                    TUPLES&& ...tuples) { }

    template <size_t POS, size_t ...INDICES>
    void operator()(std::index_sequence<POS, INDICES...>, FUNC &&func,
                  TUPLES&& ...tuples) {
        func(POS, std::get<POS>(tuples)...);
        (*this)(std::index_sequence<INDICES...>(),
                std::forward<FUNC>(func),
                std::forward<TUPLES>(tuples)...);
    }
    
};

template <typename FUNC, typename ...TUPLES>
void foreach_tuple_i(FUNC &&func, TUPLES&& ...tuples) {
    static const size_t size = tuple_sizes_equal<TUPLES...>::value;

    foreach_tuple_i_helper<FUNC, TUPLES...>()(std::make_index_sequence<size>(),
                                              std::forward<FUNC>(func),
                                              std::forward<TUPLES>(tuples)...);
}

template <typename FUNC, typename ...TUPLES>
class foreach_tuple_p_helper {
    template <typename F, typename ...T>
    friend void foreach_tuple_p(F&&, T&&...);
    
    void operator()(std::index_sequence<>, FUNC &&func,
                    TUPLES&& ...tuples) { }

    template <size_t POS, size_t ...INDICES>
    void operator()(std::index_sequence<POS, INDICES...>, FUNC &&func,
                  TUPLES&& ...tuples) {
        func.template operator()<POS>(std::get<POS>(tuples)...);
        (*this)(std::index_sequence<INDICES...>(),
                std::forward<FUNC>(func),
                std::forward<TUPLES>(tuples)...);
    }
    
};

template <typename FUNC, typename ...TUPLES>
void foreach_tuple_p(FUNC &&func, TUPLES&& ...tuples) {
    static const size_t size = tuple_sizes_equal<TUPLES...>::value;

    foreach_tuple_p_helper<FUNC, TUPLES...>()(std::make_index_sequence<size>(),
                                              std::forward<FUNC>(func),
                                              std::forward<TUPLES>(tuples)...);
}

template <typename FUNC, typename ...TUPLES>
class map_tuple_helper {
    template <typename F, typename ...T>
    friend auto map_tuple(F&&, T&&...);
    
    std::tuple<> operator()(std::index_sequence<>, FUNC &&func,
                    TUPLES&& ...tuples) { return std::tuple<>(); }

    template <size_t POS, size_t ...INDICES>
    auto operator()(std::index_sequence<POS, INDICES...>, FUNC &&func,
                  TUPLES&& ...tuples) {
        return std::tuple_cat(
            std::make_tuple(func(std::get<POS>(tuples)...)),
            (*this)(std::index_sequence<INDICES...>(),
                    std::forward<FUNC>(func),
                    std::forward<TUPLES>(tuples)...));
    }
    
};

template <typename FUNC, typename ...TUPLES>
auto map_tuple(FUNC &&func, TUPLES&& ...tuples) {
    static const size_t size = tuple_sizes_equal<TUPLES...>::value;

    return map_tuple_helper<FUNC, TUPLES...>()(
               std::make_index_sequence<size>(),
               std::forward<FUNC>(func),
               std::forward<TUPLES>(tuples)...);
}

template <typename FUNC, typename ...TUPLES>
class map_tuple_i_helper {
    template <typename F, typename ...T>
    friend auto map_tuple_i(F&&, T&&...);
    
    std::tuple<> operator()(std::index_sequence<>, FUNC &&func,
                    TUPLES&& ...tuples) { return std::tuple<>(); }

    template <size_t POS, size_t ...INDICES>
    auto operator()(std::index_sequence<POS, INDICES...>, FUNC &&func,
                  TUPLES&& ...tuples) {
        return std::tuple_cat(
            std::make_tuple(func(POS, std::get<POS>(tuples)...)),
            (*this)(std::index_sequence<INDICES...>(),
                    std::forward<FUNC>(func),
                    std::forward<TUPLES>(tuples)...));
    }
    
};

template <typename FUNC, typename ...TUPLES>
auto map_tuple_i(FUNC &&func, TUPLES&& ...tuples) {
    static const size_t size = tuple_sizes_equal<TUPLES...>::value;

    return map_tuple_i_helper<FUNC, TUPLES...>()(
               std::make_index_sequence<size>(),
               std::forward<FUNC>(func),
               std::forward<TUPLES>(tuples)...);
}

template <typename FUNC, typename ...TUPLES>
class map_tuple_p_helper {
    template <typename F, typename ...T>
    friend auto map_tuple_i(F&&, T&&...);
    
    std::tuple<> operator()(std::index_sequence<>, FUNC &&func,
                    TUPLES&& ...tuples) { return std::tuple<>(); }

    template <size_t POS, size_t ...INDICES>
    auto operator()(std::index_sequence<POS, INDICES...>, FUNC &&func,
                  TUPLES&& ...tuples) {
        return std::tuple_cat(
            std::make_tuple(
                func.template operator()<POS>(std::get<POS>(tuples)...)),
            (*this)(std::index_sequence<INDICES...>(),
                    std::forward<FUNC>(func),
                    std::forward<TUPLES>(tuples)...));
    }
    
};

template <typename FUNC, typename ...TUPLES>
auto map_tuple_p(FUNC &&func, TUPLES&& ...tuples) {
    static const size_t size = tuple_sizes_equal<TUPLES...>::value;

    return map_tuple_p_helper<FUNC, TUPLES...>()(
               std::make_index_sequence<size>(),
               std::forward<FUNC>(func),
               std::forward<TUPLES>(tuples)...);
}

template <size_t LAST_INDEX, typename FUNC, typename ...TUPLES>
class reduce_tuple_helper {
    template <typename F, typename S, typename ...T>
    friend auto reduce_tuple(F&&, S&&, T&&...);

    template <typename ACCUM>
    auto operator()(std::index_sequence<LAST_INDEX>, FUNC &&func,
                    ACCUM &&accum,
                    TUPLES&& ...tuples) {
        return func(std::forward<ACCUM>(accum),
                    std::get<LAST_INDEX>(tuples)...);
    }

    template <typename ACCUM, size_t POS, size_t ...INDICES>
    auto operator()(std::index_sequence<POS, INDICES...>,
                    FUNC &&func,
                    ACCUM &&accum,
                    TUPLES&& ...tuples) {
        return (*this)(std::index_sequence<INDICES...>(),
                       std::forward<FUNC>(func),
                       func(std::forward<ACCUM>(accum),
                            std::get<POS>(tuples)...),
                       std::forward<TUPLES>(tuples)...);
    }
};

template <typename FUNC, typename STARTING, typename ...TUPLES>
auto reduce_tuple(FUNC &&func, STARTING &&starting, TUPLES&& ...tuples) {
    static const size_t size = tuple_sizes_equal<TUPLES...>::value;

    return reduce_tuple_helper<size - 1, FUNC, TUPLES...>()(
               std::make_index_sequence<size>(),
               std::forward<FUNC>(func),
               std::forward<STARTING>(starting),
               std::forward<TUPLES>(tuples)...);
}

}

#endif
