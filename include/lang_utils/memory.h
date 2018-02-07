#ifndef LANG_UTILS_MEMORY_H
#define LANG_UTILS_MEMORY_H

#include <memory>

namespace lang_utils {

class untyped_unique_ptr {
private:
    class wrapper_base {
    public:
        virtual ~wrapper_base() {}
    };

    template <typename BASE>
    class wrapper_specialized : public wrapper_base {
    public:
        wrapper_specialized(BASE *real_ptr) : m_real_ptr(real_ptr) {}
        virtual ~wrapper_specialized() {}
        std::unique_ptr<BASE> m_real_ptr;
    };

    std::unique_ptr<wrapper_base> m_ptr;
public:
    template <typename BASE>
    untyped_unique_ptr(BASE *ptr) :
        m_ptr(static_cast<wrapper_base*>(new wrapper_specialized<BASE>(ptr))) {}

    untyped_unique_ptr(untyped_unique_ptr &&other) :
        m_ptr(std::move(other.m_ptr)) {}

    untyped_unique_ptr& operator=(untyped_unique_ptr &&other) {
        m_ptr = std::move(other.m_ptr);
    }
};

}

#endif
