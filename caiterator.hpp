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

  bool operator==(caiterator const& o) const noexcept { return o.n_ == n_; }
  bool operator!=(caiterator const&) const = default;

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

  auto operator++(int) noexcept { auto const r(*this); ++*this; return r; }
  auto operator--(int) noexcept { auto const r(*this); --*this; return r; }

  // member access
  auto operator->() const noexcept { return n_; }
  auto& operator*() const noexcept { return *n_; }

  //
  auto node() const noexcept
  {
    return const_cast<std::remove_const_t<T>*>(n_);
  }
};

}

#endif // CA_ITERATOR_HPP
