#ifndef CA_ARRAY_HPP
# define CA_ARRAY_HPP
# pragma once

#include <cstdint> // PTRDIFF_MAX
#include <algorithm> // std::move()
#include <compare> // std::three_way_comparable
#include <initializer_list>

#include "arrayiterator.hpp"

namespace ca
{

enum Method { MEMBER, NEW, USER };

struct init_t{};

template <typename T, std::size_t CAP, enum Method M = MEMBER>
  requires(
    (CAP > 0) && (CAP <= PTRDIFF_MAX) &&
    !std::is_reference_v<T> &&
    !std::is_const_v<T> &&
    std::is_default_constructible_v<T> &&
    (std::is_copy_assignable_v<T> || std::is_move_assignable_v<T>)
  ) // CAP = N - 1
class array
{
  friend class arrayiterator<T, array>;
  friend class arrayiterator<T const, array>;

public:
  using value_type = T;

  using difference_type = std::ptrdiff_t;
  using size_type = std::size_t;
  using reference = value_type&;
  using const_reference = value_type const&;

  using iterator = arrayiterator<T, array>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_iterator = arrayiterator<T const, array>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
  enum : size_type { N = CAP + 1 };

  T* f_, *l_; // pointer to first and last elements of element array
  std::conditional_t<MEMBER == M, T[N], T*> a_; // element array

  constexpr auto next(auto const p) const noexcept
  {
    return const_cast<decltype(p)>(p == &a_[N - 1] ? a_ : p + 1);
  }

  constexpr auto prev(auto const p) const noexcept
  {
    return const_cast<decltype(p)>(p == a_ ? &a_[N - 1] : p - 1);
  }

  constexpr auto next(auto const p, size_type const n) const noexcept
  { // N is the period
    size_type const d(&a_[N - 1] - p); // >= 0

    return const_cast<decltype(p)>(d >= n ? p + n : p - (N - n));
  }

  constexpr auto prev(auto const p, size_type const n) const noexcept
  {
    size_type const d(p - a_); // >= 0

    return const_cast<decltype(p)>(d >= n ? p - n : p + (N - n));
  }

public:
  constexpr array()
    noexcept(
      ((MEMBER == M) && std::is_nothrow_default_constructible_v<T[N]>) ||
      ((NEW == M) && noexcept(new T[N]))
    )
    requires((MEMBER == M) || (NEW == M))
  {
    if constexpr(NEW == M) l_ = f_ = a_ = new T[N]; else reset();
  }

  constexpr explicit array(T* const a) noexcept
    requires(USER == M):
    a_(l_ = f_ = a)
  {
  }

  constexpr array(array const& o)
    noexcept(noexcept(array(o.begin(), o.end())))
    requires(std::is_copy_assignable_v<value_type>):
    array(o.begin(), o.end())
  {
  }

  constexpr array(array&& o)
    noexcept(noexcept(*this = std::move(o)))
    requires(std::is_move_assignable_v<value_type> ||
      (NEW == M) || (USER == M))
  {
    if constexpr((MEMBER == M) || (NEW == M))
    {
      if constexpr(NEW == M) a_ = new T[N];
      reset();
    }

    *this = std::move(o);
  }

  constexpr array(init_t, auto&& ...a)
    noexcept(noexcept(push_back(std::forward<decltype(a)>(a)...))):
    array()
  {
    push_back(std::forward<decltype(a)>(a)...);
  }

  constexpr array(std::input_iterator auto const i, decltype(i) j)
    noexcept(noexcept(
      std::is_nothrow_assignable_v<value_type&, decltype(*i)>)):
    array()
  {
    std::copy(i, j, std::back_inserter(*this));
  }

  constexpr array(std::initializer_list<value_type> l)
    noexcept(noexcept(array(l.begin(), l.end()))):
    array(l.begin(), l.end())
  {
  }

  constexpr ~array()
    noexcept(((MEMBER == M) && std::is_nothrow_destructible_v<T[N]>) ||
      ((NEW == M) && noexcept(delete [] std::declval<T*>())) || (USER == M))
  {
    if constexpr(NEW == M) delete [] a_;
  }

  //
  constexpr array& operator=(array const& o)
    noexcept(noexcept(std::is_nothrow_copy_assignable_v<value_type>))
    requires(std::is_copy_assignable_v<value_type>)
  { // self-assign neglected
    clear();
    std::copy(o.begin(), o.end(), std::back_inserter(*this));
    return *this;
  }

  constexpr array& operator=(array&& o)
    noexcept(std::is_nothrow_move_assignable_v<value_type> ||
      (NEW == M) || (USER == M))
    requires(std::is_move_assignable_v<value_type> ||
      (NEW == M) || (USER == M))
  { // self-assign neglected
    if constexpr(MEMBER == M)
    {
      clear();
      std::move(o.begin(), o.end(), std::back_inserter(*this));
      o.clear();
    }
    else
    {
      f_ = o.f_; l_ = o.l_;

      if constexpr(USER == M)
      {
        a_ = o.a_; // o.a_ stays unchanged
      }
      else
      {
        std::swap(a_, o.a_);
      }

      o.reset();
    }

    return *this;
  }

  constexpr array& operator=(std::initializer_list<value_type> l)
    noexcept(noexcept(assign(l)))
    requires(std::is_copy_assignable_v<value_type>)
  {
    assign(l);
    return *this;
  }

  //
  friend constexpr bool operator==(array const& l, array const& r)
    noexcept(noexcept(std::equal(l.begin(), l.end(), r.begin(), r.end())))
  {
    return std::equal(l.begin(), l.end(), r.begin(), r.end());
  }

  friend constexpr auto operator<=>(array const& l, array const& r)
    noexcept(noexcept(std::lexicographical_compare_three_way(
      l.begin(), l.end(), r.begin(), r.end())))
  {
    return std::lexicographical_compare_three_way(
      l.begin(), l.end(), r.begin(), r.end());
  }

  //
  constexpr T* data() noexcept { return a_; }
  constexpr T const* data() const noexcept { return a_; }
  constexpr T* first() noexcept { return f_; }
  constexpr T const* first() const noexcept { return f_; }
  constexpr T* last() noexcept { return l_; }
  constexpr T const* last() const noexcept { return l_; }

  // iterators
  constexpr iterator begin() noexcept { return {this, f_}; }
  constexpr iterator end() noexcept { return {this, l_}; }

  constexpr const_iterator begin() const noexcept { return {this, f_}; }
  constexpr const_iterator end() const noexcept { return {this, l_}; }

  constexpr const_iterator cbegin() const noexcept { return {this, f_}; }
  constexpr const_iterator cend() const noexcept { return {this, l_}; }

  // reverse iterators
  constexpr reverse_iterator rbegin() noexcept
  {
    return reverse_iterator{iterator(this, l_)};
  }

  constexpr reverse_iterator rend() noexcept
  {
    return reverse_iterator{iterator(this, f_)};
  }

  // const reverse iterators
  constexpr const_reverse_iterator crbegin() const noexcept
  {
    return const_reverse_iterator{const_iterator(this, l_)};
  }

  constexpr const_reverse_iterator crend() const noexcept
  {
    return const_reverse_iterator{const_iterator{this, f_}};
  }

  //
  static constexpr size_type array_size() noexcept { return N; } // CAP + 1
  static constexpr size_type capacity() noexcept { return CAP; } // N - 1
  static constexpr size_type max_size() noexcept { return PTRDIFF_MAX; }

  //
  constexpr bool empty() const noexcept { return f_ == l_; }
  constexpr bool full() const noexcept { return next(l_) == f_; }

  constexpr size_type size() const noexcept
  {
    auto const n(l_ - f_); // N - 1 = CAP <= PTRDIFF_MAX

    return n < decltype(n){} ? N + n : n;
  }

  //
  constexpr auto& operator[](size_type const i) noexcept
  {
    return *next(f_, i);
  }

  constexpr auto const& operator[](size_type const i) const noexcept
  {
    return *next(f_, i);
  }

  constexpr auto& at(size_type const i) noexcept { return (*this)[i]; }
  constexpr auto& at(size_type const i) const noexcept { return (*this)[i]; }

  constexpr auto& back() noexcept { return *prev(l_); }
  constexpr auto const& back() const noexcept { return *prev(l_); }

  constexpr auto& front() noexcept { return *f_; }
  constexpr auto const& front() const noexcept { return *f_; }

  //
  template <int = 0>
  constexpr void assign(size_type count, auto const& v)
    noexcept(noexcept(clear(), push_back(v)))
    requires(std::is_constructible_v<value_type, decltype(v)>)
  {
    clear(); while (count--) push_back(v);
  }

  constexpr void assign(size_type const count, value_type const v)
    noexcept(noexcept(assign<0>(count, v)))
  {
    assign<0>(count, v);
  }

  constexpr void assign(std::input_iterator auto const i, decltype(i) j)
    noexcept(std::is_nothrow_assignable_v<value_type&, decltype(*i)>)
    requires(std::is_assignable_v<value_type&, decltype(*i)>)
  {
    clear();
    std::copy(i, j, std::back_inserter(*this));
  }

  constexpr void assign(std::initializer_list<value_type> l)
    noexcept(noexcept(assign(l.begin(), l.end())))
  {
    assign(l.begin(), l.end());
  }

  //
  constexpr void clear() noexcept { l_ = f_; }
  constexpr void reset() noexcept { l_ = f_ = a_; }
  constexpr void resize(size_type const n) noexcept { l_ = next(f_, n); }

  // emplacing is a bad idea in this container, avoid if possible
  constexpr void emplace_back(auto&& a)
    noexcept(noexcept(push_back(std::forward<decltype(a)>(a))))
    requires(std::is_assignable_v<value_type&, decltype(a)>)
  {
    push_back(std::forward<decltype(a)>(a));
  }

  constexpr void emplace_back(auto&& ...a)
    noexcept(noexcept(push_back(std::declval<T>())))
  {
    push_back(T(std::forward<decltype(a)>(a)...));
  }

  constexpr void emplace_back(value_type a)
    noexcept(noexcept(push_back(std::move(a))))
  {
    push_back(std::move(a));
  }

  constexpr void emplace_front(auto&& a)
    noexcept(noexcept(push_front(std::forward<decltype(a)>(a))))
    requires(std::is_assignable_v<value_type&, decltype(a)>)
  {
    push_front(std::forward<decltype(a)>(a));
  }

  constexpr void emplace_front(auto&& ...a)
    noexcept(noexcept(push_front(std::declval<T>())))
  {
    push_front(T(std::forward<decltype(a)>(a)...));
  }

  constexpr void emplace_front(value_type a)
    noexcept(noexcept(push_front(std::move(a))))
  {
    push_front(std::move(a));
  }

  constexpr auto emplace(const_iterator const i, auto&& a)
    noexcept(noexcept(insert(i, std::forward<decltype(a)>(a))))
    requires(std::is_assignable_v<value_type&, decltype(a)>)
  {
    return insert(i, std::forward<decltype(a)>(a));
  }

  constexpr auto emplace(const_iterator const i, auto&& ...a)
    noexcept(noexcept(insert(i, std::declval<T>())))
  {
    return insert(i, T(std::forward<decltype(a)>(a)...));
  }

  constexpr auto emplace(const_iterator const i, value_type a)
    noexcept(noexcept(insert(i, std::move(a))))
  {
    return insert(i, std::move(a));
  }

  //
  constexpr iterator erase(const_iterator const i)
    noexcept(std::is_nothrow_move_assignable_v<value_type>)
  {
    if (iterator const j(this, i.n()), nxt(std::next(j));
      end() == nxt) [[unlikely]]
    {
      return {this, l_ = prev(l_)};
    }
    else if (size_type(std::distance(begin(), j)) <= size() / 2)
    {
      f_ = std::move_backward(begin(), j, nxt).n();

      return nxt;
    }
    else
    {
      l_ = std::move(nxt, end(), j).n();

      return j;
    }
  }

  constexpr iterator erase(const_iterator a, const_iterator const b)
    noexcept(noexcept(erase(a)))
  {
    while (a != b) { a = erase(a); } return a;
  }

  //
  template <int = 0>
  constexpr iterator insert(const_iterator const i, auto&& v)
    noexcept(std::is_nothrow_assignable_v<value_type&, decltype(v)>)
    requires(std::is_assignable_v<value_type&, decltype(v)>)
  {
    if (full()) [[unlikely]] pop_front();

    //
    T* n;

    if (iterator const j(this, i.n()); end() == j) [[unlikely]]
    {
      l_ = next(n = l_);
    }
    else if (size_type(std::distance(begin(), j)) <= size() / 2)
    {
      auto const f(f_);
      f_ = prev(f);

      n = std::move({this, f}, j, begin()).n();
    }
    else
    {
      n = j.n();

      auto const l(l_);
      l_ = next(l);

      std::move_backward(j, {this, l}, end());
    }

    //
    *n = std::forward<decltype(v)>(v);

    return {this, n};
  }

  constexpr auto insert(const_iterator const i, value_type v)
    noexcept(noexcept(insert<0>(i, std::move(v))))
  {
    return insert<0>(i, std::move(v));
  }

  template <int = 0>
  constexpr iterator insert(const_iterator i, size_type count,
    auto const& v)
    noexcept(noexcept(insert(i, v)))
    requires(std::is_assignable_v<value_type&, decltype(v)>)
  {
    if (count) [[likely]]
    { // i is invalidated after insert, but r is valid
      auto const r(insert(i, v));
      i = std::next(r);

      while (--count) i = std::next(insert(i, v));

      return r;
    }
    else [[unlikely]]
    {
      return {a_, i.n()};
    }
  }

  constexpr iterator insert(const_iterator i, size_type const count,
    value_type const v)
    noexcept(noexcept(insert<0>(i, count, v)))
  {
    return insert<0>(i, count, v);
  }

  constexpr iterator insert(const_iterator i,
    std::input_iterator auto const j, decltype(j) k)
    noexcept(noexcept(insert(i, *j)))
  {
    if (j == k) [[unlikely]]
    {
      return {a_, i.n()};
    }
    else [[likely]]
    { // i is invalidated after insert, but r is valid
      auto const r(emplace(i, *j));
      i = std::next(r);

      std::for_each(
        std::next(j),
        k,
        [&](auto&& v)
          noexcept(noexcept(insert(i, std::forward<decltype(v)>(v))))
        {
          i = std::next(insert(i, std::forward<decltype(v)>(v)));
        }
      );

      return r;
    }
  }

  //
  constexpr void pop_back() noexcept { l_ = prev(l_); }
  constexpr void pop_back(size_type const n) noexcept { l_ = prev(l_, n); }
  constexpr void pop_front() noexcept { f_ = next(f_); }
  constexpr void pop_front(size_type const n) noexcept { f_ = next(f_, n); }

  //
  template <int = 0>
  constexpr void push_back(auto&& a)
    noexcept(std::is_nothrow_assignable_v<value_type&, decltype(a)>)
    requires(std::is_assignable_v<value_type&, decltype(a)>)
  {
    *l_ = std::forward<decltype(a)>(a);
    if ((l_ = next(l_)) == f_) [[unlikely]] pop_front();
  }

  void push_back(auto&& ...v)
    noexcept(noexcept((push_back<0>(std::forward<decltype(v)>(v)), ...)))
    requires(requires{(push_back<0>(std::forward<decltype(v)>(v)), ...);})
  {
    (push_back<0>(std::forward<decltype(v)>(v)), ...);
  }

  constexpr void push_back(value_type a)
    noexcept(noexcept(push_back<0>(std::move(a))))
  {
    push_back<0>(std::move(a));
  }

  template <int = 0>
  constexpr void push_front(auto&& v)
    noexcept(std::is_nothrow_assignable_v<value_type&, decltype(v)>)
    requires(std::is_assignable_v<value_type&, decltype(v)>)
  {
    *(full() ? f_ : f_ = prev(f_)) = std::forward<decltype(v)>(v);
  }

  void push_front(auto&& ...v)
    noexcept(noexcept((push_front<0>(std::forward<decltype(v)>(v)), ...)))
    requires(requires{(push_front<0>(std::forward<decltype(v)>(v)), ...);})
  {
    (push_front<0>(std::forward<decltype(v)>(v)), ...);
  }

  constexpr void push_front(value_type v)
    noexcept(noexcept(push_front<0>(std::move(v))))
  {
    push_front<0>(std::move(v));
  }

  //
  constexpr void swap(array& o) noexcept requires((NEW == M) || (USER == M))
  {
    std::swap(f_, o.f_);
    std::swap(l_, o.l_);
    std::swap(a_, o.a_);
  }
};

//////////////////////////////////////////////////////////////////////////////
template <typename T, std::size_t S, enum Method M>
constexpr auto erase_if(array<T, S, M>& c, auto pred)
  noexcept(noexcept(c.erase(c.begin()), pred(std::declval<T&>())))
{
  typename std::remove_reference_t<decltype(c)>::size_type r{};

  for (auto i(c.begin()); i.n() != c.last();
    pred(*i) ? ++r, i = c.erase(i) : ++i);

  return r;
}

template <int = 0, typename T, std::size_t S, enum Method M>
constexpr auto erase(array<T, S, M>& c, auto const& ...k)
  noexcept(noexcept(c.erase(c.begin()),
    (std::equal_to<>()(std::declval<T&>(), k), ...)))
  requires(requires{(std::equal_to<>()(std::declval<T&>(), k), ...);})
{
  return erase_if(
      c,
      [eq(std::equal_to<>()), &k...](auto&& v)
        noexcept(noexcept((std::declval<std::equal_to<>>()(
          std::forward<decltype(v)>(v), k), ...)))
      {
        return (eq(std::forward<decltype(v)>(v), k) || ...);
      }
    );
}

template <typename T, std::size_t S, enum Method M>
constexpr auto erase(array<T, S, M>& c, T const k)
  noexcept(noexcept(erase<0>(c, k)))
{
  return erase<0>(c, k);
}

//////////////////////////////////////////////////////////////////////////////
template <typename T, std::size_t S, enum Method M>
constexpr void swap(array<T, S, M>& lhs, decltype(lhs) rhs) noexcept
  requires((NEW == M) || (USER == M))
{
  lhs.swap(rhs);
}

//////////////////////////////////////////////////////////////////////////////
constexpr void split(std::random_access_iterator auto const b, decltype(b) e,
  auto&& g)
  noexcept(noexcept(g(b.n(), b.n())))
{
  auto f(b.n());
  auto const l(e.n());

  if (l < f)
  {
    auto const d(b.a());

    g(f, &d[std::remove_const_t<decltype(b)>::N]); // f > l >= d, f > d
    f = d;
  }

  g(f, l);
}

}

#endif // CA_ARRAY_HPP
