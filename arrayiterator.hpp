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

  CA const* a_;
  T* n_;

public:
  using iterator_category = std::random_access_iterator_tag;
  using difference_type = std::ptrdiff_t;
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
    auto const d([a(a_->a_), f(a_->f_)](auto const p) noexcept
      {
        auto d(p - f);

        if (d < 0) d = (&a[CA::cap - 1] - f) + p - a + 1;

        return d;
      }
    );

    //
    return d(n_) - d(o.n_);
  }

  constexpr arrayiterator operator+(std::size_t const N) const noexcept
  {
    return {a_, a_->next(n_, N)};
  }

  constexpr arrayiterator operator-(std::size_t const N) const noexcept
  {
    return {a_, a_->prev(n_, N)};
  }

  constexpr auto& operator+=(std::size_t const N) noexcept
  {
    n_ = a_->next(n_, N); return *this;
  }

  constexpr auto& operator-=(std::size_t const N) noexcept
  {
    n_ = a_->prev(n_, N); return *this;
  }

  // comparison
  constexpr bool operator==(arrayiterator const& o) const noexcept
  {
    return n_ == o.n_;
  }

  constexpr bool operator<(arrayiterator const& o) const noexcept
  {
    return *this - o < 0;
  }

  // member access
  constexpr auto& operator[](std::size_t const i) const noexcept
  {
    return *a_->next(n_, i);
  }

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
