#ifndef CA_ARRAYCONTRIBFRIENDS_HPP
# define CA_ARRAYCONTRIBFRIENDS_HPP
# pragma once

#ifdef NO_CA_ARRAY_CONTRIB

#define CA_ARRAYCONTRIB_FRIENDS /* empty */

#else

// needed for dst->l_ in arraycontrib.hpp
#define CA_ARRAYCONTRIB_FRIENDS                                                                                                                                  \
    template <typename CA_ARR_T, typename CA_ARR_CA, typename CA_ARR_CA2>                                                                                          \
    friend constexpr std::back_insert_iterator<CA_ARR_CA2> std::copy(ca::arrayiterator<CA_ARR_T const, CA_ARR_CA> first, ca::arrayiterator<CA_ARR_T const, CA_ARR_CA> last, std::back_insert_iterator<CA_ARR_CA2> d_first)
//                                                          |
//                                                          !

#include "arrayiterator.hpp"

namespace std {

template <typename T, typename CA, typename OutputIt>
constexpr OutputIt copy(ca::arrayiterator<T const, CA> beg, ca::arrayiterator<T const, CA> end, OutputIt dst);


template <typename T, typename T2, typename CA, typename CA2>
constexpr ca::arrayiterator<T2, CA> copy(ca::arrayiterator<T const, CA> beg, ca::arrayiterator<T const, CA> end, ca::arrayiterator<T2, CA2> dst);


template <typename T, typename CA, typename CA2>
constexpr std::back_insert_iterator<CA2> copy(ca::arrayiterator<T const, CA> beg, ca::arrayiterator<T const, CA> end, std::back_insert_iterator<CA2> dst1);

} // namespace std

#endif // NO_CA_ARRAY_OVERRIDES

#endif // CA_ARRAYCONTRIBFRIENDS_HPP
