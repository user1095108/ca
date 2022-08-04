#ifndef CA_ARRAYITERATOR_HPP
# define CA_ARRAYITERATOR_HPP
# pragma once

#include <iterator>
#include <type_traits>

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

  T* n_, *a_;

public:
  using iterator_category = std::bidirectional_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using value_type = T;

  using pointer = value_type*;
  using reference = value_type&;

public:
  arrayiterator() = default;

  constexpr arrayiterator(T* const a, decltype(a) n) noexcept: n_(n), a_(a) { }

  constexpr arrayiterator(arrayiterator const&) = default;
  constexpr arrayiterator(arrayiterator&&) = default;

  // iterator -> const_iterator conversion
  constexpr arrayiterator(inverse_const_t const& o) noexcept
    requires(std::is_const_v<T>):
    n_(o.n_),
    a_(o.a_)
  {
  }

  // assignment
  constexpr arrayiterator& operator=(arrayiterator const&) = default;
  constexpr arrayiterator& operator=(arrayiterator&&) = default;

  // increment, decrement
  constexpr auto& operator++() noexcept
  {
    n_ = CA::S::next(a_, n_); return *this;
  }

  constexpr auto& operator--() noexcept
  {
    n_ = CA::S::prev(a_, n_); return *this;
  }

  constexpr arrayiterator operator++(int) noexcept
  {
    auto const n(n_); n_ = CA::S::next(a_, n); return {a_, n};
  }

  constexpr arrayiterator operator--(int) noexcept
  {
    auto const n(n_); n_ = CA::S::prev(a_, n); return {a_, n};
  }

  // comparison
  constexpr bool operator==(arrayiterator const& o) const noexcept
  {
    return n_ == o.n_;
  }

  // member access
  constexpr auto operator->() const noexcept { return n_; }
  constexpr auto& operator*() const noexcept { return *n_; }

  //
  constexpr auto n() const noexcept
  {
    return const_cast<std::remove_const_t<T>*>(n_);
  }
};

}

#endif // CA_ARRAYITERATOR_HPP
