#ifndef CA_ITERATOR_HPP
# define CA_ITERATOR_HPP
# pragma once

#include <iterator>
#include <type_traits>

namespace ca
{

template <typename T, typename CA>
class caiterator
{
  using inverse_const_t = std::conditional_t<
    std::is_const_v<CA>,
    caiterator<std::remove_const_t<T>, std::remove_const_t<CA>>,
    caiterator<T const, CA const>
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
  caiterator() = default;

  constexpr caiterator(T* const a, T* const n) noexcept:
    n_(n),
    a_(a)
  {
  }

  constexpr caiterator(caiterator const&) = default;
  constexpr caiterator(caiterator&&) = default;

  // iterator -> const_iterator conversion
  constexpr caiterator(inverse_const_t const& o) noexcept
    requires(std::is_const_v<CA>):
    n_(o.n_),
    a_(o.a_)
  {
  }

  // assignment
  constexpr caiterator& operator=(caiterator const&) = default;
  constexpr caiterator& operator=(caiterator&&) = default;

  // increment, decrement
  constexpr auto& operator++() noexcept
  {
    n_ = std::remove_const_t<CA>::next(a_, n_); return *this;
  }

  constexpr auto& operator--() noexcept
  {
    n_ = std::remove_const_t<CA>::prev(a_, n_); return *this;
  }

  constexpr caiterator operator++(int) noexcept
  {
    auto const n(n_);
    n_ = std::remove_const_t<CA>::next(a_, n);
    return {a_, n};
  }

  constexpr caiterator operator--(int) noexcept
  {
    auto const n(n_);
    n_ = std::remove_const_t<CA>::prev(a_, n);
    return {a_, n};
  }

  // comparison
  constexpr bool operator==(caiterator const& o) const noexcept
  {
    return o.n_ == n_;
  }

  bool operator!=(caiterator const&) const = default;

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

#endif // CA_ITERATOR_HPP
