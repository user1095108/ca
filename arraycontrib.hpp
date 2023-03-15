#ifndef CA_ARRAYOVERRIDES_HPP
# define CA_ARRAYOVERRIDES_HPP
# pragma once

#ifndef NO_CA_ARRAY_CONTRIB

#include <algorithm>
#include <cassert>
#include "arrayiterator.hpp"


namespace {
template<typename C>
class ca_back_insert_iterator : public std::back_insert_iterator<C> {
public:
    C *getContainer() { return this->container; }
};
} // anon namespace



namespace std {

template <typename T, typename CA, typename OutputIt>
constexpr OutputIt copy(ca::arrayiterator<T const, CA> beg, ca::arrayiterator<T const, CA> end, OutputIt dst)
{
    split(beg, end, [&dst](T const * f, T const *l) {
        dst = std::copy(f, l, dst);
    });
    return dst;
}


template <typename T, typename T2, typename CA, typename CA2>
constexpr ca::arrayiterator<T2, CA> copy(ca::arrayiterator<T const, CA> beg, ca::arrayiterator<T const, CA> end, ca::arrayiterator<T2, CA2> dst)
{

    return ca::iterViaRaw(beg, end, dst, [](auto b, auto e, auto dst) { std::copy(b, e, dst); });
}


template <typename T, typename CA, typename CA2>
constexpr std::back_insert_iterator<CA2> copy(ca::arrayiterator<T const, CA> beg, ca::arrayiterator<T const, CA> end, std::back_insert_iterator<CA2> dst1)
{
    auto dst = static_cast<ca_back_insert_iterator<CA2> &>(dst1).getContainer();

    dst->l_ = copy(beg, end, dst->end()).n();

    return dst1;
}

} // namespace std

#endif // NO_CA_ARRAY_OVERRIDES

#endif // CA_ARRAYOVERRIDES_HPP
