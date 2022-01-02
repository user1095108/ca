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

  T* n_;
  CA* ca_;

public:
  using iterator_category = std::bidirectional_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using value_type = T;

  using pointer = value_type*;
  using reference = value_type&;

public:
  caiterator() = default;

  constexpr caiterator(CA* const ca, T* const n) noexcept:
    n_(n),
    ca_(ca)
  {
  }

  constexpr caiterator(caiterator const&) = default;
  constexpr caiterator(caiterator&&) = default;

  // iterator -> const_iterator conversion
  constexpr caiterator(inverse_const_t const& o) noexcept
    requires(std::is_const_v<CA>):
    n_(o.n_),
    ca_(o.ca_)
  {
  }

  //
  constexpr caiterator& operator=(caiterator const&) = default;
  constexpr caiterator& operator=(caiterator&&) = default;

  constexpr bool operator==(caiterator const& o) const noexcept
  {
    return o.n_ == n_;
  }

  bool operator!=(caiterator const&) const = default;

  // increment, decrement
  constexpr auto& operator++() noexcept
  {
    n_ = ca_->next(ca_->a_, n_);
    return *this;
  }

  constexpr auto& operator--() noexcept
  {
    n_ = ca_->prev(ca_->a_, n_);
    return *this;
  }

  constexpr auto operator++(int) noexcept
  {
    auto const r(*this); ++*this; return r;
  }

  constexpr auto operator--(int) noexcept
  {
    auto const r(*this); --*this; return r;
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

#endif // CA_ITERATOR_HPP
