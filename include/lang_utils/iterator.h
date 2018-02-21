#ifndef LANG_UTILS_ITERATOR_H
#define LANG_UTILS_ITERATOR_H

#include <iterator>
#include <type_traits>
#include <cassert>
#include <memory>
#include <functional>

namespace lang_utils {

template <typename VALUE>
class dynamic_iterator {
public:
    using value_type = typename std::decay<VALUE>::type;
    using reference_type = typename std::add_lvalue_reference<value_type>::type;
    using pointer_type = typename std::add_pointer<value_type>::type;

    //for now, make this adaptable later
    using iterator_category = std::bidirectional_iterator_tag;

private:
    class wrapper_base {
    public:
        virtual wrapper_base* clone() = 0;
        virtual void increment() = 0;
        virtual void decrement() = 0;
        virtual reference_type dereference() = 0;

        //re-enable these when I deal with random access
        //virtual int difference(const wrapper_base &other) const = 0;
        //virtual void advance(const int offset) = 0;
        virtual bool equal(const wrapper_base &other) const = 0;
    };

    template <typename BASE>
    class wrapper_derived : public wrapper_base {
    public:
        wrapper_derived(BASE &&base) : m_base(base) {}
        wrapper_derived(const BASE &base) : m_base(base) {}
        
        virtual wrapper_base* clone() {
            return (wrapper_base*)new wrapper_derived<BASE>(m_base);
        }
        
        virtual void increment() { ++m_base; }
        virtual void decrement() { ++m_base; }
        virtual reference_type dereference() { return *m_base; }

        //re-enable these when I deal with random access
        /*virtual int difference(const wrapper_base &other) {
            const wrapper_derived<BASE> *other_p =
                dynamic_cast<const wrapper_derived<BASE>*>(&other);
            assert(other_p);
            return m_base - other_p->m_base;
            }*/

        /*virtual void advance(const size_t offset) {
           m_base += offset;
           }*/

        virtual bool equal(const wrapper_base &other) const {
            const wrapper_derived<BASE> *other_p =
                dynamic_cast<const wrapper_derived<BASE>*>(&other);

            return other_p != nullptr && m_base == other_p->m_base;
        }

    private:
        BASE m_base;
    };

    using wrapper_ptr = std::unique_ptr<wrapper_base>;
public:

    dynamic_iterator() : m_wrapper(nullptr) {}

    dynamic_iterator(const dynamic_iterator &other)
        : m_wrapper(other.m_wrapper->clone()) {}

    dynamic_iterator(dynamic_iterator &&other)
        : m_wrapper(std::move(other.m_wrapper)) {}
        
    template <typename BASE>
    dynamic_iterator(BASE &&base)
        : m_wrapper((wrapper_base*)
                      new wrapper_derived<typename std::decay<BASE>::type>(
                          std::forward<BASE>(base))) {}

    dynamic_iterator& operator=(const dynamic_iterator &other) {
        m_wrapper = wrapper_ptr(other.m_wrapper->clone());
        return *this;
    }
    
    dynamic_iterator& operator=(dynamic_iterator &&other) {
        m_wrapper = std::move(other.m_wrapper);
        return *this;
    }

    dynamic_iterator& operator++() {
        m_wrapper->increment();
        return *this;
    }

    dynamic_iterator& operator--() {
        m_wrapper->decrement();
        return *this;
    }

    reference_type operator*() {
        return m_wrapper->dereference();
    }

    int operator-(const dynamic_iterator &other) const {
        return m_wrapper->difference(*other->m_wrapper);
    }

    dynamic_iterator& operator+(const int offset) {
        m_wrapper->advance(offset);
        return *this;
    }

    bool operator==(const dynamic_iterator &other) const {
        return m_wrapper->equal(*other.m_wrapper);
    }
    bool operator!=(const dynamic_iterator &other) const {
        return !m_wrapper->equal(*other.m_wrapper);
    }
private:
    wrapper_ptr m_wrapper;
};

template <typename BASE>
dynamic_iterator<typename std::iterator_traits<BASE>::value_type>
make_dynamic_iterator(BASE &&base) {
    return dynamic_iterator<typename std::iterator_traits<BASE>::value_type>(
               std::forward<BASE>(base));
}

template <typename VALUE>
class dynamic_collection {
public:
    dynamic_collection() {}
    template <typename COLLECTION>

    dynamic_collection(COLLECTION &&collection) {
        set(collection);
    }

    dynamic_collection(dynamic_collection &&other)
        : m_get_begin(std::move(other.m_get_begin)),
          m_get_end(std::move(other.m_get_end)) {}
    
    dynamic_collection(const dynamic_collection &other)
        : m_get_begin(other.m_get_begin),
          m_get_end(other.m_get_end) {}
    
    using iterator = dynamic_iterator<VALUE>;

    template <typename COLLECTION>
    void set(COLLECTION &collection) {
        static_assert(std::is_same<
           typename std::iterator_traits<
                      typename COLLECTION::iterator>::value_type,
           VALUE>::value);
        m_get_begin = [&collection]() -> iterator {
            return make_dynamic_iterator(collection.begin());
        };
        m_get_end = [&collection]() -> iterator {
            return make_dynamic_iterator(collection.end());
        };
    }

    iterator begin() { return m_get_begin(); }
    iterator end() { return m_get_end(); }
    

private:
    std::function<iterator()> m_get_begin;
    std::function<iterator()> m_get_end;
};

}

#endif
