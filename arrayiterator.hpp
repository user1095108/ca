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
  using difference_type = typename CA::difference_type;
  using size_type = typename CA::size_type;
  using value_type = T;

  using pointer = value_type*;
  using reference = value_type&;

  enum : size_type { N = CA::N };

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

  constexpr auto operator<=>(arrayiterator const& o) const noexcept
  {
    auto const d([f(a_->f_)](auto const p) noexcept -> difference_type
      {
        auto const d(p - f);

        return d + (d < 0 ? size_type(CA::N) : size_type{});
      }
    );

    //
    return d(n_) <=> d(o.n_);
  }

  // member access
  constexpr auto& operator[](difference_type const n) const noexcept
  {
    return *(n >= 0 ? a_->next(n_, n) : a_->prev(n_, -n));
  }

  constexpr auto operator->() const noexcept { return n_; }
  constexpr auto& operator*() const noexcept { return *n_; }

  //
  constexpr auto a() const noexcept
  {
    return const_cast<std::remove_const_t<T>*>(a_->a_);
  }

  constexpr auto n() const noexcept
  {
    return const_cast<std::remove_const_t<T>*>(n_);
  }
};

//////////////////////////////////////////////////////////////////////////////
template <typename T, typename CA>
constexpr auto operator+(typename CA::difference_type const n,
  arrayiterator<T, CA> const i) noexcept
{
  return i + n;
}

}

#endif // CA_ARRAYITERATOR_HPP
