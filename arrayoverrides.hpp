#ifndef CA_ARRAYOVERRIDES_HPP
# define CA_ARRAYOVERRIDES_HPP
# pragma once

#ifndef NO_CA_ARRAY_OVERRIDES


#include <algorithm>
#include <cassert>
#include "arrayiterator.hpp"

namespace ca
{

template <typename T, typename CA>
class arrayiterator;

struct CopyToLim
{
    const unsigned char *       b;
    const unsigned char *       lim;
          unsigned char *       b_o;
          unsigned char * const start_o;
    const unsigned char * const edge_o;
};

unsigned char * copy_to_lim(CopyToLim &args);

} // namespace ca


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
    ca::iterViaRaw(beg, end, [&dst](T const * f, T const *l) {
        dst = std::copy(f, l, dst);
    });

    return dst;
}


template <typename T, typename T2, typename CA, typename CA2>
constexpr ca::arrayiterator<T2, CA> copy(ca::arrayiterator<T const, CA> beg, ca::arrayiterator<T const, CA> end, ca::arrayiterator<T2, CA2> dst)
{
    assert(beg.a_ == end.a_);
    auto const num = end-beg;
    assert(num <= CA2::capacity() - dst.a_->size());

    const unsigned char * const e = reinterpret_cast<const unsigned char *>(end.n_);
    ca::CopyToLim args {
        reinterpret_cast<const unsigned char *>(beg.n_),                                              // b       (sourc begin)
        reinterpret_cast<const unsigned char *>(e),                                                   // lim     (sourc lim)
        reinterpret_cast<      unsigned char *>(dst.n_),                                              // b_o     (dest begin)
        reinterpret_cast<      unsigned char *>(const_cast<T2 *>(dst.a_->data())),                    // start_o (dest &a_[0])
        reinterpret_cast<const unsigned char *>(               &(dst.a_->data()[CA2::array_size()])), // edge_o  (dest &a_[N])
    };

    if (e < args.b) {
        auto const edge = reinterpret_cast<const unsigned char *>(&(beg.a_->data()[CA::array_size()]));
        args.lim = edge;
        args.b_o = ca::copy_to_lim(args); // copy (end of array) during wrap around

        args.b = reinterpret_cast<const unsigned char *>(beg.a_->data()); // b copied up to edge, now set to beginning of array
        args.lim = e;
    }
    decltype(dst.n_) b_o = reinterpret_cast<decltype(b_o)>(ca::copy_to_lim(args)); // copy

    return {dst.a_, b_o};
}


template <typename T, typename CA, typename CA2>
constexpr std::back_insert_iterator<CA2> copy(ca::arrayiterator<T const, CA> beg, ca::arrayiterator<T const, CA> end, std::back_insert_iterator<CA2> dst1)
{
    auto dst = static_cast<ca_back_insert_iterator<CA2> &>(dst1).getContainer();

    dst->l_ = copy(beg, end, dst->end()).n_;

    return dst1;
}

} // namespace std

#endif // NO_CA_ARRAY_OVERRIDES

#endif // CA_ARRAYOVERRIDES_HPP
