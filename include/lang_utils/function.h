#ifndef LANG_UTILS_FUNCTION_H
#define LANG_UTILS_FUNCTION_H

#include <type_traits>
#include <utility>

namespace lang_utils {

template <typename OBJ, typename ENABLE = void>
struct construct;

template <typename OBJ>
struct construct<OBJ,
                 typename std::enable_if<!std::is_pointer<OBJ>::value>::type> {
    template <typename ...ARGS>
    OBJ operator()(ARGS &&...args) {
        return OBJ(std::forward<ARGS>(args)...);
    }
};

template <typename PTR>
struct construct<PTR,
                 typename std::enable_if<std::is_pointer<PTR>::value>::type> {
    template <typename ...ARGS>
    PTR operator()(ARGS &&...args) {
        using OBJ = typename std::remove_pointer<PTR>::type;
        return new OBJ(std::forward<ARGS>(args)...);
    }
};

}

#endif
