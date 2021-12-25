#ifndef CA_CA_HPP
# define CA_CA_HPP
# pragma once

#include <algorithm> // inplace_merge()
#include <utility> // as_const()

#include "caiterator.hpp"

namespace ca
{

enum Method { MEMBER, NEW };

template <typename T, std::size_t N, enum Method M = MEMBER>
class array
{
  static_assert(N);

  friend class caiterator<T, array>;
  friend class caiterator<T const, array const>;

public:
  using value_type = T;

  using difference_type = std::ptrdiff_t;
  using size_type = std::size_t;
  using reference = value_type&;
  using const_reference = value_type const&;

  using const_iterator = caiterator<T const, array const>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  using iterator = caiterator<T, array>;
  using reverse_iterator = std::reverse_iterator<iterator>;

private:
  T* first_, *last_;
  std::size_t sz_{};

  std::conditional_t<
    MEMBER == M,
    T[N],
    std::conditional_t<NEW == M, T*, void>
  > a_;

  template <difference_type I>
  auto next(auto const p) noexcept requires ((1 == I) || (-1 == I))
  {
    if constexpr((N & (N - 1)) && (1 == I))
      return p == &a_[N - 1] ? &*a_ : p + 1;
    else if constexpr((N & (N - 1)) && (-1 == I))
      return p == &*a_ ? &a_[N - 1] : p - 1;
    else
      return &a_[(p - &*a_ + I) & (N - 1)];
  }

  template <difference_type I>
  auto next(auto const p) const noexcept requires ((1 == I) || (-1 == I))
  {
    if constexpr((N & (N - 1)) && (1 == I))
      return p == &a_[N - 1] ? &*a_ : p + 1;
    else if constexpr((N & (N - 1)) && (-1 == I))
      return p == &*a_ ? &a_[N - 1] : p - 1;
    else
      return &a_[(p - &*a_ + I) & (N - 1)];
  }

  //
  static void sort(auto const b, decltype(b) e, size_type const sz,
    auto&& cmp)
  {
    if (sz > 1)
    {
      auto const hsz(sz / 2);
      auto const m(std::next(b, hsz));

      sort(b, m, hsz, std::forward<decltype(cmp)>(cmp));
      sort(m, e, sz - hsz, std::forward<decltype(cmp)>(cmp));

      std::inplace_merge(b, m, e, cmp);
    }
  }

public:
  array()
    noexcept(
      ((MEMBER == M) && std::is_nothrow_default_constructible_v<T[N]>) ||
      ((NEW == M) && noexcept(new T[N]))
    )
  {
    if constexpr(NEW == M) a_ = new T[N];
    first_ = last_ = a_;
  }

  ~array()
    noexcept(
      ((MEMBER == M) && std::is_nothrow_default_constructible_v<T[N]>) ||
      ((NEW == M) && noexcept(new T[N]))
    )
  {
    if constexpr(NEW == M) delete [] a_;
  }

  //
  array(array const& o)
    noexcept(noexcept(*this = o))
    requires(std::is_copy_assignable_v<T>)
  {
    *this = o;
  }

  array(array&& o)
    noexcept(noexcept(*this = std::move(o)))
    requires(std::is_move_assignable_v<T>)
  {
    *this = std::move(o);
  }

  // self-assign neglected
  array& operator=(array const& o)
    noexcept(std::is_nothrow_copy_assignable_v<T>)
    requires(std::is_copy_assignable_v<T>)
  {
    std::copy(o.cbegin(), o.cend(), a_);

    first_ = &a_[o.first - o.a_]; last_ = &a_[o.last_ - o.a_];
    sz_ = o.sz_;

    return *this;
  }

  array& operator=(array&& o)
    noexcept(std::is_nothrow_move_assignable_v<T>)
    requires(std::is_move_assignable_v<T>)
  {
    if constexpr(MEMBER == M)
    {
      std::move(o.begin(), o.end(), a_);
      first_ = &a_[o.first - o.a_]; last_ = &a_[o.last_ - o.a_];
    }
    else
    {
      first_ = o.first_;
      std::swap(last_, o.last_);
      std::swap(a_, o.a_);
    }

    sz_ = o.sz_;

    //
    o.sz_ = {};
    o.first_ = o.last_;

    return *this;
  }

  //
  friend bool operator==(array const& lhs, array const& rhs) noexcept
  {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
  }

  friend auto operator<=>(array const& lhs, array const& rhs) noexcept
  {
    return std::lexicographical_compare_three_way(
      lhs.begin(), lhs.end(), rhs.begin(), rhs.end()
    );
  }

  friend bool operator!=(array const&, array const&) = default;
  friend bool operator<(array const&, array const&) = default;
  friend bool operator<=(array const&, array const&) = default;
  friend bool operator>(array const&, array const&) = default;
  friend bool operator>=(array const&, array const&) = default;

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
  auto max_size() const noexcept { return ~std::size_t(); }
  auto size() const noexcept { return sz_; }

  //
  auto& operator[](size_type const i) noexcept
  {
    return *std::next(begin(), i);
  }

  auto& operator[](size_type const i) const noexcept
  {
    return *std::next(begin(), i);
  }

  //
  auto& back() noexcept { return *last_; }
  auto& back() const noexcept { return std::as_const(*last_); }

  auto& front() noexcept { return *first_; }
  auto& front() const noexcept { return std::as_const(*first_); }

  //
  iterator erase(const_iterator const i)
    noexcept(std::is_nothrow_move_assignable_v<T>)
  {
    if (--sz_)
    {
      iterator const j(this, i.n());
      auto const nxt(std::next(j));

      if (std::distance(begin(), j) <= std::distance(nxt, end()))
      {
        std::move_backward(begin(), j, nxt);
        first_ = next<1>(first_);

        return nxt;
      }
      else
      {
        std::move(nxt, end(), j);
        last_ = next<-1>(last_);

        return nxt.n() ? j : nxt;
      }
    }
    else
    {
      return end();
    }
  }

  iterator erase(const_iterator a, const_iterator const b)
    noexcept(noexcept(erase(a)))
  {
    iterator i(b);

    for (; a != b; i = erase(a), a = i);

    return i;
  }

  iterator erase(std::initializer_list<const_iterator> il)
    noexcept(noexcept(erase(cbegin())))
  {
    iterator r;

    std::for_each(il.begin(), il.end(), [&](auto const i) { r = erase(i); });

    return r;
  }

  //
  void pop_back() noexcept { if (--sz_) { last_ = next<-1>(last_); } }
  void pop_front() noexcept { if (--sz_) { first_ = next<1>(first_); } }

  //
  void push(const_iterator const i, auto&& v)
    noexcept(std::is_nothrow_assignable_v<T, decltype(v)&&>)
  {
    if (full())
    {
      --sz_;
      first_ = next<1>(first_);
    }

    T* n;

    do
    {
      if (size())
      {
        iterator const j(this, i.n());

        if (std::distance(begin(), j) <= std::distance(j, end()))
        {
          auto const f(first_);
          first_ = next<-1>(f);

          n = std::move(iterator(this, f), j, begin()).n();
          break;
        }
        else if (last_ = next<1>(last_); j.n())
        {
          n = j.n();
          std::move_backward(j, {this, last_}, end());
          break;
        }
      }

      n = last_;
    } while (false);

    //
    *n = std::forward<decltype(v)>(v);
    ++sz_;
  }

  void push_back(auto&& v)
    noexcept(std::is_nothrow_assignable_v<T, decltype(v)&&>)
  {
    if (full())
    {
      first_ = next<1>(last_ = first_);
    }
    else if (sz_++)
    {
      last_ = next<1>(last_);
    }

    //
    *last_ = std::forward<decltype(v)>(v);
  }

  void push_front(auto&& v)
    noexcept(std::is_nothrow_assignable_v<T, decltype(v)&&>)
  {
    if (!full() && sz_++)
    {
      first_ = next<-1>(first_);
    }

    //
    *first_ = std::forward<decltype(v)>(v);
  }

  //
  void reverse() noexcept { std::swap(first_, last_); }
  void sort() { sort(std::less<value_type>()); }
  void sort(auto cmp) { sort(begin(), end(), size(), cmp); }

  void swap(array& o) noexcept requires(NEW == M)
  {
    std::swap(first_, o.first_);
    std::swap(last_, o.last_);
    std::swap(sz_, o.sz_);
    std::swap(a_, o.a_);
  }

  //
  friend auto erase(array& c, auto const& k)
    noexcept(std::is_nothrow_move_assignable_v<T>)
  {
    return erase_if(c, [&](auto&& v) noexcept{return std::equal_to()(v, k);});
  }

  friend auto erase_if(array& c, auto pred)
    noexcept(std::is_nothrow_move_assignable_v<T>)
  {
    size_type r{};

    for (auto i(c.begin()); i.n();)
    {
      i = pred(*i) ? (++r, c.erase(i)) : std::next(i);
    }

    return r;
  }

  friend void sort(auto const b, decltype(b) e)
    requires(std::is_same_v<iterator, std::remove_const_t<decltype(b)>> ||
      std::is_same_v<reverse_iterator, std::remove_const_t<decltype(b)>>)
  {
    sort(b, e, std::distance(b, e), std::less<value_type>());
  }

  friend void sort(auto const b, decltype(b) e, auto cmp)
    requires(std::is_same_v<iterator, std::remove_const_t<decltype(b)>> ||
      std::is_same_v<reverse_iterator, std::remove_const_t<decltype(b)>>)
  {
    sort(b, e, std::distance(b, e), cmp);
  }

  friend void swap(array& lhs, decltype(lhs) rhs) noexcept requires(NEW == M)
  {
    lhs.swap(rhs);
  }
};

}

#endif // CA_CA_HPP
