#ifndef LANG_UTILS_TUPLE_H
#define LANG_UTILS_TUPLE_H

#include <tuple>
#include <type_traits>
#include <utility>

namespace lang_utils {

template <template <typename> typename, typename> struct transform_tuple_type;

template <template <typename> typename TRANSFORM, typename... ARGS>
struct transform_tuple_type<TRANSFORM, std::tuple<ARGS...>> {
    using type = std::tuple<typename TRANSFORM<ARGS>::type...>;
};

template <size_t N>
constexpr size_t check_array_equal(size_t const (&arr)[N]) {
    size_t ret = 0;
    for (size_t s : arr) {
        ret = (s == arr[0]) ? s : throw "Sizes mismatch";
    }
    return ret;
}

template <typename... TUPLES>
static constexpr size_t const tuple_sizes_equal_v =
    check_array_equal({std::tuple_size_v<std::decay_t<TUPLES>>...});

template <typename ...TUPLES>
struct tuple_sizes_equal : public
    std::integral_constant<size_t, tuple_sizes_equal_v<TUPLES...>> {};

template <size_t N, typename... TUPLES>
auto slice_tuples(TUPLES&&... tuples) {
    return std::tie(std::get<N>(std::forward<TUPLES>(tuples))...);
}

enum class pos_kind {
    none,
    compile_time,
    run_time
};

template <pos_kind, size_t, typename> struct adapted;

template <size_t I, typename FUNC>
struct adapted<pos_kind::none, I, FUNC> {
    FUNC&& func;
    template <typename... ARGS>
    decltype(auto) operator()(ARGS&&... args) const {
        return std::forward<FUNC>(func)(std::forward<ARGS>(args)...);
    }
};

template <size_t I, typename FUNC>
struct adapted<pos_kind::compile_time, I, FUNC> {
    FUNC&& func;
    template <typename... ARGS>
    decltype(auto) operator()(ARGS&&... args) const {
        return std::forward<FUNC>(func).template operator()<I>(std::forward<ARGS>(args)...);
    }
};

template <size_t I, typename FUNC>
struct adapted<pos_kind::run_time, I, FUNC> {
    FUNC&& func;
    template <typename... ARGS>
    decltype(auto) operator()(ARGS&&... args) const {
        return std::forward<FUNC>(func)(I, std::forward<ARGS>(args)...);
    }
};

template <pos_kind PK, size_t I, typename FUNC>
adapted<PK, I, FUNC&&> adapt_impl(FUNC&& func) {
    return {std::forward<FUNC>(func)};
}

template <pos_kind PK, typename FUNC, size_t... INDS, typename... TUPLES>
void foreach_tuple_impl(FUNC&& func, std::index_sequence<INDS...>,
                        TUPLES&&... tuples) {
    static_cast<void>((... && (static_cast<void>(
        std::apply(adapt_impl<PK, INDS>(std::forward<FUNC>(func)),
                   slice_tuples<INDS>(std::forward<TUPLES>(tuples)...))), true)));
}

template <typename FUNC, typename... TUPLES>
void foreach_tuple(FUNC&& func, TUPLES&&... tuples) {
    foreach_tuple_impl<pos_kind::none>(
            std::forward<FUNC>(func),
            std::make_index_sequence<tuple_sizes_equal_v<TUPLES...>>{},
            std::forward<TUPLES>(tuples)...);
}

template <typename FUNC, size_t... INDS, typename... TUPLES>
void foreach_tuple_i(FUNC&& func, TUPLES&&... tuples) {
    foreach_tuple_impl<pos_kind::run_time>(
            std::forward<FUNC>(func),
            std::make_index_sequence<tuple_sizes_equal_v<TUPLES...>>{},
            std::forward<TUPLES>(tuples)...);
}

template <typename FUNC, typename ...TUPLES>
void foreach_tuple_p(FUNC &&func, TUPLES&& ...tuples) {
    foreach_tuple_impl<pos_kind::compile_time>(
            std::forward<FUNC>(func),
            std::make_index_sequence<tuple_sizes_equal_v<TUPLES...>>{},
            std::forward<TUPLES>(tuples)...);
}

template <pos_kind PK, typename FUNC, size_t... INDS, typename... TUPLES>
auto map_tuple_impl(FUNC&& func,
                    std::index_sequence<INDS...>,
                    TUPLES&&... tuples) {
    return std::make_tuple(
            std::apply(adapt_impl<PK, INDS>(std::forward<FUNC>(func)),
                       slice_tuples<INDS>(std::forward<TUPLES>(tuples)...))...);
}

template <typename FUNC, typename ...TUPLES>
auto map_tuple(FUNC &&func, TUPLES&& ...tuples) {
    return map_tuple_impl<pos_kind::none>(
        std::forward<FUNC>(func),
        std::make_index_sequence<tuple_sizes_equal_v<TUPLES...>>{},
        std::forward<TUPLES>(tuples)...);
}

template <typename FUNC, typename ...TUPLES>
auto map_tuple_i(FUNC &&func, TUPLES&& ...tuples) {
    return map_tuple_impl<pos_kind::run_time>(
        std::forward<FUNC>(func),
        std::make_index_sequence<tuple_sizes_equal_v<TUPLES...>>{},
        std::forward<TUPLES>(tuples)...);
}

template <typename FUNC, typename ...TUPLES>
auto map_tuple_p(FUNC &&func, TUPLES&& ...tuples) {
    return map_tuple_impl<pos_kind::compile_time>(
        std::forward<FUNC>(func),
        std::make_index_sequence<tuple_sizes_equal_v<TUPLES...>>{},
        std::forward<TUPLES>(tuples)...);
}


template <size_t LAST_INDEX, typename FUNC, typename ...TUPLES>
struct reduce_tuple_helper {
    // clang doesn't like template friends.
    // template <typename F, typename S, typename ...T>
    // friend auto reduce_tuple(F&&, S&&, T&&...);

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
