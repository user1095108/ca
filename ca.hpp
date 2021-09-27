#ifndef CA_CA_HPP
# define CA_CA_HPP
# pragma once

#include <utility>

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
  circular_array() noexcept { first_ = last_ = &*a_; }

  //
  circular_array(circular_array const& o) noexcept(noexcept(*this = o))
  {
    *this = o;
  }

  circular_array(circular_array&& o) noexcept(noexcept(*this = std::move(o)))
  {
    *this = std::move(o);
  }

  //
  circular_array& operator=(circular_array const& o) noexcept(
    std::is_nothrow_copy_assignable_v<T>)
  {
    std::copy(o.cbegin(), o.cend(), a_);

    first_ = a_ + (o.first - o.a_); last_ = a_ + (o.last_ - o.a_);
    sz_ = o.sz_;

    return *this;
  }

  circular_array& operator=(circular_array&& o) noexcept(
    std::is_nothrow_move_assignable_v<T>)
  {
    std::move(o.begin(), o.end(), a_);

    first_ = a_ + (o.first - o.a_); last_ = a_ + (o.last_ - o.a_);
    sz_ = o.sz_;

    o.first_ = o.last_ = {}; o.sz_ = {};

    return *this;
  }

  // iterators
  iterator begin() noexcept { return {this, size() ? first_ : nullptr}; }
  iterator end() noexcept { return {this, {}}; }

  const_iterator begin() const noexcept
  {
    return {this, size() ? first_ : nullptr};
  }

  const_iterator end() const noexcept { return {this, {}}; }

  const_iterator cbegin() const noexcept
  {
    return {this, size() ? first_ : nullptr};
  }

  const_iterator cend() const noexcept { return {this, {}}; }

  // reverse iterators
  reverse_iterator rbegin() noexcept
  {
    return reverse_iterator{iterator(this, {})};
  }

  reverse_iterator rend() noexcept
  {
    return reverse_iterator{iterator(this, size() ? first_ : nullptr)};
  }

  // const reverse iterators
  const_reverse_iterator crbegin() const noexcept
  {
    return const_reverse_iterator{const_iterator(this, {})};
  }

  const_reverse_iterator crend() const noexcept
  {
    return const_reverse_iterator{
      const_iterator{this, size() ? first_ : nullptr}
    };
  }

  //
  void clear() noexcept { first_ = last_; sz_ = {}; }
  bool empty() const noexcept { return !size(); }
  bool full() const noexcept { return N == size(); }

  std::size_t size() const noexcept { return sz_; }

  //
  auto& back() noexcept { assert(sz_); return *last_; }
  auto& back() const noexcept { assert(sz_); return std::as_const(*last_); }

  auto& front() noexcept {  assert(sz_); return *first_; }
  auto& front() const noexcept { assert(sz_); return std::as_const(*first_); }

  //
  iterator erase(const_iterator i) noexcept(
    std::is_nothrow_move_assignable_v<T>)
  {
    iterator r(this, &*a_ + (i.node() - &*a_)), j;

    auto const end(this->end());

    if (auto const tmp(std::next(i)); end != tmp)
    {
      j = iterator(this, &*a_ + (tmp.node() - &*a_));

      do
      {
        *std::prev(j) = std::move(*j);
        j = std::next(j);
      }
      while (end != j);

      last_ = prev(last_);
      --sz_;
    }

    return r;
  }

  iterator erase(const_iterator a, const_iterator const b)
  {
    iterator i(b);

    for (; a != b; i = erase(a), a = i);

    return i;
  }

  //
  void pop_back() noexcept
  {
    assert(sz_);
    if (--sz_)
    {
      last_ = prev(last_);
    }
  }

  void pop_front() noexcept
  {
    assert(sz_);
    if (--sz_)
    {
      first_ = next(first_);
    }
  }

  //
  void push_back(T const& v)
  {
    if (full())
    {
      pop_front();
    }

    if (auto const l(last_); empty())
    {
      assert(first_ == last_);
      *l = v;
    }
    else
    {
      *(last_ = next(l)) = v;
    }

    ++sz_;
  }

  void push_back(T&& v) noexcept
  {
    if (full())
    {
      pop_front();
    }

    if (auto const l(last_); empty())
    {
      assert(first_ == last_);
      *l = std::move(v);
    }
    else
    {
      *(last_ = next(l)) = std::move(v);
    }

    ++sz_;
  }

  //
  void push_front(T const& v)
  {
    if (auto const f(first_); full())
    {
      *f = v;
    }
    else
    {
      if (empty())
      {
        assert(first_ == last_);
        *f = v;
      }
      else
      {
        *(first_ = prev(f)) = v;
      }

      ++sz_;
    }
  }

  void push_front(T&& v) noexcept
  {
    if (auto const f(first_); full())
    {
      *f = std::move(v);
    }
    else
    {
      if (empty())
      {
        assert(first_ == last_);
        *f = std::move(v);
      }
      else
      {
        *(first_ = prev(f)) = std::move(v);
      }

      ++sz_;
    }
  }

  //
  friend bool operator!=(circular_array const&,
    circular_array const&) = default;
  friend bool operator<(circular_array const&,
    circular_array const&) = default;
  friend bool operator<=(circular_array const&,
    circular_array const&) = default;
  friend bool operator>(circular_array const&,
    circular_array const&) = default;
  friend bool operator>=(circular_array const&,
    circular_array const&) = default;
};

template <typename V, std::size_t N>
inline bool operator==(circular_array<V, N> const& lhs,
  circular_array<V, N> const& rhs) noexcept 
{
  return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename V, std::size_t N>
inline auto operator<=>(circular_array<V, N> const& lhs,
  circular_array<V, N> const& rhs) noexcept
{
  return std::lexicographical_compare_three_way(
    lhs.begin(), lhs.end(), rhs.begin(), rhs.end()
  );
}

template <typename V, std::size_t N>
constexpr auto erase(circular_array<V, N>& c, auto const& k)
{
  return erase_if(c, [&](auto&& v) noexcept {return std::equal_to()(v, k);});
}

template <typename V, std::size_t N>
constexpr auto erase_if(circular_array<V, N>& c, auto pred)
{
  typename circular_array<V, N>::size_type r{};

  auto const end(c.end());

  for (auto i(c.begin()); end != i;)
  {
    i = pred(*i) ? (++r, c.erase(i)) : std::next(i);
  }

  return r;
}

}

#endif // CA_CA_HPP
