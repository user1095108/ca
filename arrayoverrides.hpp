#ifndef CA_ARRAYOVERRIDES_HPP
# define CA_ARRAYOVERRIDES_HPP
# pragma once

#ifdef NO_CA_ARRAY_OVERRIDES
#define CA_ARRAYOVERRIDES_FRIENDS /* empty */
#else

#define CA_ARRAYOVERRIDES_FRIENDS                                                                                                                                  \
    template <typename CA_ARR_T, typename CA_ARR_CA, typename OutputIt>                                                                                            \
    friend constexpr OutputIt std::copy(ca::arrayiterator<CA_ARR_T const, CA_ARR_CA> first, ca::arrayiterator<CA_ARR_T const, CA_ARR_CA> last, OutputIt d_first ); \
                                                                                                                                                                   \
    template <typename CA_ARR_T, typename CA_ARR_T2, typename CA_ARR_CA, typename CA_ARR_CA2>                                                                      \
    friend constexpr ca::arrayiterator<CA_ARR_T2, CA_ARR_CA> std::copy(ca::arrayiterator<CA_ARR_T const, CA_ARR_CA> first, ca::arrayiterator<CA_ARR_T const, CA_ARR_CA> last, ca::arrayiterator<CA_ARR_T2, CA_ARR_CA2> d_first); \
                                                                                                                                                                   \
    template <typename CA_ARR_T, typename CA_ARR_CA, typename CA_ARR_CA2>                                                                                          \
    friend constexpr std::back_insert_iterator<CA_ARR_CA2> std::copy(ca::arrayiterator<CA_ARR_T const, CA_ARR_CA> first, ca::arrayiterator<CA_ARR_T const, CA_ARR_CA> last, std::back_insert_iterator<CA_ARR_CA2> d_first)
//                                                          |
//                                                          !

#include <algorithm>
#include <cassert>

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
constexpr OutputIt copy(ca::arrayiterator<T const, CA> first, ca::arrayiterator<T const, CA> last, OutputIt dst)
{
    assert(first.a_ == last.a_);

    auto       b    = first.n_;
    auto const e    = last.n_;

    if (e < b) {
        auto const edge = &(first.a_->a_[CA::N]);
        dst = std::copy(b, edge, dst);  // copy (end of array) during wrap around
        b = first.a_->a_;
    }
    return std::copy(b, e, dst); // copy
}


template <typename T, typename T2, typename CA, typename CA2>
constexpr ca::arrayiterator<T2, CA> copy(ca::arrayiterator<T const, CA> beg, ca::arrayiterator<T const, CA> end, ca::arrayiterator<T2, CA2> dst)
{
    assert(beg.a_ == end.a_);
    auto const num = end-beg;
    assert(num <= CA2::capacity() - dst.a_->size());

    const unsigned char * const e = reinterpret_cast<const unsigned char *>(end.n_);
    ca::CopyToLim args {
        reinterpret_cast<const unsigned char *>(beg.n_),                               // b       (sourc begin)
        reinterpret_cast<const unsigned char *>(e),                                    // lim     (sourc lim)
        reinterpret_cast<      unsigned char *>(dst.n_),                               // b_o     (dest begin)
        reinterpret_cast<      unsigned char *>(const_cast<T2 *>(dst.a_->data())),         // start_o (dest &a_[0])
        reinterpret_cast<const unsigned char *>(               &(dst.a_->data()[CA2::N])), // edge_o  (dest &a_[N])
    };

    if (e < args.b) {
        auto const edge = reinterpret_cast<const unsigned char *>(&(beg.a_->data()[CA::N]));
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
