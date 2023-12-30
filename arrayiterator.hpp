#ifndef CA_ARRAYITERATOR_HPP
# define CA_ARRAYITERATOR_HPP
# pragma once

#include <iterator>
#include <type_traits>
#include <utility>

namespace ca
{

enum Method { MEMBER, NEW, USER };

namespace detail
{

constexpr auto assign(auto& ...a) noexcept
{ // assign idiom
  return [&](auto const ...v) noexcept { [](...)noexcept{}((a = v)...); };
}

}

template <typename T, typename CA>
class arrayiterator
{
  using iterator_t = arrayiterator<std::remove_const_t<T>, CA>;
  friend arrayiterator<T const, CA>;

  template <typename U, std::size_t CAP, enum Method>
    requires(
      !std::is_reference_v<U> &&
      !std::is_const_v<U> &&
      std::is_default_constructible_v<U>
//    (CAP > 0) && (CAP <= PTRDIFF_MAX)
//    (std::is_copy_assignable_v<U> || std::is_move_assignable_v<U>)
    )
  friend class array;

  CA const* a_;
  std::remove_const_t<T>* n_;

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
    n_(decltype(n_)(n))
  {
  }

  constexpr arrayiterator(arrayiterator const&) = default;
  constexpr arrayiterator(arrayiterator&&) = default;

  // iterator -> const_iterator conversion
  constexpr arrayiterator(iterator_t const& o) noexcept
    requires(std::is_const_v<T>):
    a_(o.a_),
    n_(o.n_)
  {
  }

  // assignment
  constexpr arrayiterator& operator=(arrayiterator const&) = default;
  constexpr arrayiterator& operator=(arrayiterator&&) = default;

  constexpr arrayiterator& operator=(iterator_t const& o) noexcept
    requires(std::is_const_v<T>)
  {
    a_ = o.a_; n_ = o.n_; return *this;
  }

  // conversion to bool
  constexpr explicit operator bool() const noexcept { return n_ != a_->l_; }

  // increment, decrement
  constexpr auto& operator++() noexcept { n_ = a_->next_(n_); return *this; }
  constexpr auto& operator--() noexcept { n_ = a_->prev_(n_); return *this; }

  constexpr arrayiterator operator++(int) noexcept
  {
    auto const n(n_); n_ = a_->next_(n); return {a_, n};
  }

  constexpr arrayiterator operator--(int) noexcept
  {
    auto const n(n_); n_ = a_->prev_(n); return {a_, n};
  }

  // arithmetic
  constexpr auto operator-(arrayiterator const& o) const noexcept
  {
    auto const f(a_->f_);
    return difference_type(CA::distance_(f, n_) - CA::distance_(f, o.n_));
  }

  constexpr arrayiterator operator+(difference_type const n) const noexcept
  {
    return {a_, a_->adv_(n_, n)};
  }

  constexpr arrayiterator operator-(difference_type const n) const noexcept
  {
    return {a_, a_->adv_(n_, -n)};
  }

  constexpr auto& operator+=(difference_type const n) noexcept
  {
    n_ = a_->adv_(n_, n); return *this;
  }

  constexpr auto& operator-=(difference_type const n) noexcept
  {
    n_ = a_->adv_(n_, -n); return *this;
  }

  // comparison
  constexpr bool operator==(arrayiterator const& o) const noexcept
  {
    return n_ == o.n_;
  }

  constexpr auto operator<=>(arrayiterator const& o) const noexcept
  {
    decltype(n_) const f(a_->f_);
    return CA::distance_(f, n_) <=> CA::distance_(f, o.n_);
  }

  // member access
  constexpr auto& operator[](difference_type const n) const noexcept
  {
    return *a_->adv_(n_, n);
  }

  constexpr auto operator->() const noexcept
  {
    return std::add_pointer_t<T>(n_);
  }

  constexpr auto& operator*() const noexcept
  {
    return *std::add_pointer_t<T>(n_);
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
