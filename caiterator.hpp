#ifndef CA_ITERATOR_HPP
# define CA_ITERATOR_HPP
# pragma once

#include <cassert>

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

  T* n_{};
  CA* ca_{};

public:
  using iterator_category = std::bidirectional_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using value_type = T;

  using pointer = value_type*;
  using reference = value_type&;

public:
  caiterator() = default;

  caiterator(CA* ca, T* const n) noexcept:
    n_(n),
    ca_(ca)
  {
  }

  caiterator(caiterator const&) = default;
  caiterator(caiterator&&) = default;

  // iterator -> const_iterator conversion
  caiterator(inverse_const_t const& o) noexcept requires(std::is_const_v<CA>):
    n_(o.n_),
    ca_(o.ca_)
  {
  }

  //
  caiterator& operator=(caiterator const&) = default;
  caiterator& operator=(caiterator&&) = default;

  bool operator==(auto const& o) const noexcept
    requires(
      std::is_same_v<std::remove_cvref_t<decltype(o)>, caiterator> ||
      std::is_same_v<std::remove_cvref_t<decltype(o)>, inverse_const_t>
    )
  {
    return n_ == o.n_;
  }

  bool operator!=(auto&& o) const noexcept
  {
    return !(*this == std::forward<decltype(o)>(o));
  }

  // increment, decrement
  auto& operator++() noexcept
  {
    return n_ = n_ == ca_->last_ ? nullptr : ca_->next(n_), *this;
  }

  auto& operator--() noexcept
  {
    return n_ = !n_ ? ca_->last_ :
      n_ == ca_->first_ ? nullptr : ca_->prev(n_), *this;
  }

  auto operator++(int) noexcept { auto r(*this); return ++*this, r; }
  auto operator--(int) noexcept { auto r(*this); return --*this, r; }

  // member access
  auto operator->() const noexcept { return n_; }
  auto& operator*() const noexcept { return *n_; }

  //
  std::size_t index() const noexcept { return n_ - ca_->a_; }
  auto node() const noexcept { return n_; }
};

}

#endif // CA_ITERATOR_HPP
