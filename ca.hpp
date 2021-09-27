#ifndef CA_CA_HPP
# define CA_CA_HPP
# pragma once

#include "caiterator.hpp"

namespace ca
{

template <typename T, std::size_t N>
class circular_array
{
  friend class caiterator<T, circular_array>;
  friend class caiterator<T const, circular_array const>;

  using value_type = T;

  using difference_type = std::ptrdiff_t;
  using size_type = std::size_t;
  using reference = value_type&;
  using const_reference = value_type const&;

  using const_iterator = caiterator<T const, circular_array const>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  using iterator = caiterator<T, circular_array>;
  using reverse_iterator = std::reverse_iterator<iterator>;

  T* first_, *last_;
  std::size_t sz_{};

  T a_[N];

  auto next(auto const p) noexcept
  {
    return p == &a_[N - 1] ? &*a_ : p + 1;
  }

  auto prev(auto const p) noexcept
  {
    return p == &*a_ ? &a_[N - 1] : p - 1;
  }

  auto next(auto const p) const noexcept
  {
    return p == &a_[N - 1] ? &*a_ : p + 1;
  }

  auto prev(auto const p) const noexcept
  {
    return p == &*a_ ? &a_[N - 1] : p - 1;
  }

public:
  circular_array() = default;
  circular_array(circular_array const&) = default;
  circular_array(circular_array&&) = default;

  // iterators
  iterator begin() noexcept { return {this, first_}; }
  iterator end() noexcept { return {this, {}}; }

  const_iterator begin() const noexcept { return {this, first_}; }
  const_iterator end() const noexcept { return {this, {}}; }

  const_iterator cbegin() const noexcept { return {this, first_}; }
  const_iterator cend() const noexcept { return {this, {}}; }

  // reverse iterators
  reverse_iterator rbegin() noexcept
  {
    return reverse_iterator{iterator(this, {})};
  }

  reverse_iterator rend() noexcept
  {
    return reverse_iterator{iterator(this, first_)};
  }

  // const reverse iterators
  const_reverse_iterator crbegin() const noexcept
  {
    return const_reverse_iterator{const_iterator(this, {})};
  }

  const_reverse_iterator crend() const noexcept
  {
    return const_reverse_iterator{const_iterator{this, first_}};
  }

  //
  void clear() noexcept { first_ = last_; sz_ = {}; }
  bool empty() const noexcept { return !size(); }
  bool full() const noexcept { return N == size(); }

  std::size_t size() const noexcept { return sz_; }

  //
  void pop_back() noexcept { last_ = prev(last_); --sz_; }
  void pop_front() noexcept { first_ = next(first_); --sz_; }

  //
  void push_back(T const& v)
  {
    if (full())
    {
      pop_front();
    }

    if (empty())
    {
      *(first_ = last_ = &*a_) = v;
    }
    else
    {
      *(last_ = next(last_)) = v;
    }

    ++sz_;
  }

  void push_back(T&& v) noexcept
  {
    if (full())
    {
      pop_front();
    }

    if (empty())
    {
      *(first_ = last_ = &*a_) = std::move(v);
    }
    else
    {
      *(last_ = next(last_)) = std::move(v);
    }

    ++sz_;
  }

  //
  void push_front(T const& v)
  {
    if (full())
    {
      pop_front();
    }

    if (empty())
    {
      *(first_ = last_ = &*a_) = v;
    }
    else
    {
      *(first_ = prev(first_)) = v;
    }

    ++sz_;
  }

  void push_front(T&& v) noexcept
  {
    if (full())
    {
      pop_front();
    }

    if (empty())
    {
      *(first_ = last_ = &*a_) = std::move(v);
    }
    else
    {
      *(first_ = prev(first_)) = std::move(v);
    }

    ++sz_;
  }
};

}

#endif // CA_CA_HPP
