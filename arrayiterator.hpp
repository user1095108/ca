#ifndef CA_ARRAYITERATOR_HPP
# define CA_ARRAYITERATOR_HPP
# pragma once

#include <iterator>
#include <type_traits>
#include <cassert>

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


namespace ca
{

template <typename T, typename CA>
class arrayiterator
{
  using inverse_const_t = std::conditional_t<
    std::is_const_v<T>,
    arrayiterator<std::remove_const_t<T>, CA>,
    arrayiterator<T const, CA>
  >;

  friend inverse_const_t;

  CA_ARRAYOVERRIDES_FRIENDS;


  template <typename CA_ARR_T, typename CA_ARR_CA>
  friend constexpr void iterViaRaw(const arrayiterator<CA_ARR_T const, CA_ARR_CA> & beg, const arrayiterator<CA_ARR_T const, CA_ARR_CA> & end, auto&& g);

  CA const* a_;
  T* n_;

public:
  using iterator_category = std::random_access_iterator_tag;
  using difference_type = typename CA::difference_type;
  using size_type = typename CA::size_type;
  using value_type = T;

  using pointer = value_type*;
  using reference = value_type&;

public:
  arrayiterator() = default;

  constexpr arrayiterator(CA const* const a, T* const n) noexcept:
    a_(a),
    n_(n)
  {
  }

  constexpr arrayiterator(arrayiterator const&) = default;
  constexpr arrayiterator(arrayiterator&&) = default;

  // iterator -> const_iterator conversion
  constexpr arrayiterator(inverse_const_t const& o) noexcept
    requires(std::is_const_v<T>):
    a_(o.a_),
    n_(o.n_)
  {
  }

  // assignment
  constexpr arrayiterator& operator=(arrayiterator const&) = default;
  constexpr arrayiterator& operator=(arrayiterator&&) = default;

  // increment, decrement
  constexpr auto& operator++() noexcept { n_ = a_->next(n_); return *this; }
  constexpr auto& operator--() noexcept { n_ = a_->prev(n_); return *this; }

  constexpr arrayiterator operator++(int) noexcept
  {
    auto const n(n_); n_ = a_->next(n); return {a_, n};
  }

  constexpr arrayiterator operator--(int) noexcept
  {
    auto const n(n_); n_ = a_->prev(n); return {a_, n};
  }

  // arithmetic
  constexpr auto operator-(arrayiterator const& o) const noexcept
  {
    auto const d([f(a_->f_)](auto const p) noexcept -> difference_type
      {
        auto const d(p - f);

        return d + (d < 0 ? size_type(CA::N) : size_type{});
      }
    );

    //
    return d(n_) - d(o.n_);
  }

  constexpr arrayiterator operator+(difference_type const n) const noexcept
  {
    return {a_, n >= 0 ? a_->next(n_, n) : a_->prev(n_, -n)};
  }

  constexpr arrayiterator operator-(difference_type const n) const noexcept
  {
    return {a_, n >= 0 ? a_->prev(n_, n) : a_->next(n_, -n)};
  }

  constexpr auto& operator+=(difference_type const n) noexcept
  {
    n_ = n >= 0 ? a_->next(n_, n) : a_->prev(n_, -n); return *this;
  }

  constexpr auto& operator-=(difference_type const n) noexcept
  {
    n_ = n >= 0 ? a_->prev(n_, n) : a_->next(n_, -n); return *this;
  }

  // comparison
  constexpr bool operator==(arrayiterator const& o) const noexcept
  {
    return n_ == o.n_;
  }

  constexpr bool operator<(arrayiterator const& o) const noexcept
  {
    auto const d([f(a_->f_)](auto const p) noexcept -> difference_type
      {
        auto const d(p - f);

        return d + (d < 0 ? size_type(CA::N) : size_type{});
      }
    );

    //
    return d(n_) < d(o.n_);
  }

  // member access
  constexpr auto& operator[](difference_type const n) const noexcept
  {
    return *(n >= 0 ? a_->next(n_, n) : a_->prev(n_, -n));
  }

  constexpr auto operator->() const noexcept { return n_; }
  constexpr auto& operator*() const noexcept { return *n_; }

  //
  constexpr auto n() const noexcept
  {
    return const_cast<std::remove_const_t<T>*>(n_);
  }

};


template <typename T>
constexpr void iterRawConst(const T *f, const T *l, const T *dataStart, const T *dataEnd, auto&& g)
{
  if (l - f < 0)
  {
    g(f, dataEnd);
    f = dataStart;
  }
  g(f, l);
}

template <typename T, typename CA>
constexpr void iterViaRaw(const arrayiterator<T const, CA> & beg, const arrayiterator<T const, CA> & end, auto&& g)
{
  assert(beg.a_ == end.a_);

  auto const d(beg.a_->data());
  iterRawConst(beg.n_, end.n_, d, &d[CA::array_size()], g);
}

} // namespace ca




#endif // CA_ARRAYITERATOR_HPP

