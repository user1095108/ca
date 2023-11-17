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

struct multi_t{};

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

//private:
  enum : size_type { N = CAP + 1 };

  T* f_, *l_; // pointer to first and last elements of element array
  std::conditional_t<MEMBER == M, T[N], T*> a_; // element array

  static constexpr auto assign_(auto& ...a) noexcept
  { // assign idiom
    return [&](auto const ...v) noexcept { ((a = v), ...); };
  }

  constexpr auto next_(auto const p) const noexcept
  {
    return const_cast<decltype(p)>(p == &a_[N - 1] ? a_ : p + 1);
  }

  constexpr auto prev_(auto const p) const noexcept
  {
    return const_cast<decltype(p)>(p == a_ ? &a_[N - 1] : p - 1);
  }

  constexpr auto next_(auto const p, difference_type const n) const noexcept
  { // 0 <= n < N, N is the period
    auto const u(N - n);
    return const_cast<decltype(p)>(size_type(p - a_) < u ? p + n : p - u);
  }

  constexpr auto prev_(auto const p, difference_type const n) const noexcept
  { // 0 <= n < N
    return const_cast<decltype(p)>(p - a_ < n ? p + (N - n) : p - n);
  }

  constexpr auto adv_(auto const p, difference_type const n) const noexcept
  { // -N < n < N
    if (size_type const o(p - a_); difference_type(o) < -n)
    {
      return const_cast<decltype(p)>(p + (N + n));
    }
    else
    {
      auto const u(N - n);
      return const_cast<decltype(p)>(o < u ? p + n : p - u);
    }
  }

  static constexpr size_type distance_(auto const a, decltype(a) b) noexcept
  {
    auto const n(b - a); // N - 1 = CAP <= PTRDIFF_MAX
    return n < decltype(n){} ? N + n : n;
  }

public:
  constexpr array()
    noexcept(std::is_nothrow_default_constructible_v<T[N]>)
    requires(MEMBER == M): a_()
  {
    f_ = l_ = a_;
  }

  constexpr array() noexcept(noexcept(new T[N])) requires(NEW == M)
  {
    f_ = l_ = a_ = new T[N];
  }

  constexpr explicit array(T* const a) noexcept requires(USER == M)
  {
    f_ = l_ = a_ = a;
  }

  constexpr array(array const& o)
    noexcept(noexcept(array(o.begin(), o.end())))
    requires(std::is_copy_assignable_v<value_type>):
    array(o.begin(), o.end())
  {
  }

  constexpr array(array&& o)
    noexcept((NEW == M) || (USER == M) ||
      noexcept(std::move(o.begin(), o.end(), std::back_inserter(*this))))
    requires((NEW == M) || (USER == M) ||
      std::is_move_assignable_v<value_type>):
    array()
  {
    if constexpr(MEMBER == M)
    {
      std::move(o.begin(), o.end(), std::back_inserter(*this));
      o.clear();
    }
    else
    {
      if constexpr(USER == M)
      {
        f_ = o.f_; l_ = o.l_; a_ = o.a_; // o.a_ stays unchanged
        o.clear();
      }
      else
      { // swap & o.reset()
        assign_(f_, l_, a_, o.f_, o.l_, o.a_)(o.f_, o.l_, o.a_, a_, a_, a_);
      }
    }
  }

  constexpr array(multi_t, auto&& ...a)
    noexcept(noexcept(push_back(std::forward<decltype(a)>(a)...))):
    array()
  {
    push_back(std::forward<decltype(a)>(a)...);
  }

  constexpr array(std::input_iterator auto const i, decltype(i) j)
    noexcept(noexcept(std::copy(i, j, std::back_inserter(*this)))):
    array()
  {
    std::copy(i, j, std::back_inserter(*this));
  }

  constexpr array(std::initializer_list<value_type> l)
    noexcept(noexcept(array(l.begin(), l.end()))):
    array(l.begin(), l.end())
  {
  }
 
  constexpr explicit array(size_type const c)
    noexcept(noexcept(array(), resize(c))):
    array()
  {
    resize(c);
  }

  constexpr explicit array(size_type const c, auto const& v, multi_t = {})
    noexcept(noexcept(array(c), std::fill(f_, l_, v)))
    requires(std::is_assignable_v<value_type&, decltype(v)>):
    array(c)
  {
    std::fill(f_, l_, v);
  }

  constexpr explicit array(size_type const c, value_type const v)
    noexcept(noexcept(array(c, v, multi_t{}))):
    array(c, v, multi_t{})
  {
  }

  constexpr explicit array(auto&& c)
    noexcept((std::is_rvalue_reference_v<decltype(c)> &&
      noexcept(std::move(std::begin(c), std::end(c),
        std::back_inserter(*this)))) ||
      noexcept(std::copy(std::begin(c), std::end(c),
        std::back_inserter(*this))))
    requires(requires{std::begin(c), std::end(c), std::size(c);} &&
      !std::same_as<array, std::remove_cvref_t<decltype(c)>> &&
      !std::same_as<std::initializer_list<value_type>,
        std::remove_cvref_t<decltype(c)>> &&
      (std::is_assignable_v<T&, decltype(*std::begin(c))> ||
      std::is_assignable_v<T&, decltype(std::move(*std::begin(c)))>)):
    array()
  {
    if constexpr(std::is_rvalue_reference_v<decltype(c)>)
    {
      std::move(std::begin(c), std::end(c), std::back_inserter(*this));
    }
    else
    {
      std::copy(std::begin(c), std::end(c), std::back_inserter(*this));
    }
  }

  constexpr ~array() requires(NEW != M) = default;

  constexpr ~array() noexcept(noexcept(delete [] a_)) requires(NEW == M)
  {
    delete [] a_;
  }

  //
  constexpr array& operator=(array const& o)
    noexcept(noexcept(
      std::copy(o.begin(), o.end(), std::back_inserter(*this))))
    requires(std::is_copy_assignable_v<value_type>)
  { // self-assign neglected
    clear(); std::copy(o.begin(), o.end(), std::back_inserter(*this));

    return *this;
  }

  constexpr array& operator=(array&& o)
    noexcept((NEW == M) || (USER == M) ||
      noexcept(std::move(o.begin(), o.end(), std::back_inserter(*this))))
    requires((NEW == M) || (USER == M) ||
      std::is_move_assignable_v<value_type>)
  { // self-assign neglected
    if constexpr(MEMBER == M)
    {
      clear();
      std::move(o.begin(), o.end(), std::back_inserter(*this));
      o.clear();
    }
    else
    {
      if constexpr(USER == M)
      {
        f_ = o.f_; l_ = o.l_; a_ = o.a_; // o.a_ stays unchanged
        o.clear();
      }
      else
      { // swap & o.reset()
        assign_(f_, l_, a_, o.f_, o.l_, o.a_)(o.f_, o.l_, o.a_, a_, a_, a_);
      }
    }

    return *this;
  }

  constexpr array& operator=(std::initializer_list<value_type> l)
    noexcept(noexcept(assign(l)))
    requires(std::is_copy_assignable_v<value_type>)
  {
    assign(l); return *this;
  }

  auto& operator=(auto&& c)
    noexcept((std::is_rvalue_reference_v<decltype(c)> &&
      noexcept(clear(), std::move(std::begin(c), std::end(c),
        std::back_inserter(*this)))) ||
      noexcept(clear(), std::copy(std::begin(c), std::end(c),
        std::back_inserter(*this))))
    requires(requires{std::begin(c), std::end(c), std::size(c);} &&
      !std::same_as<array, std::remove_cvref_t<decltype(c)>> &&
      !std::same_as<std::initializer_list<value_type>,
        std::remove_cvref_t<decltype(c)>> &&
      (std::is_assignable_v<T&, decltype(*std::begin(c))> ||
      std::is_assignable_v<T&, decltype(std::move(*std::begin(c)))>))
  {
    clear();

    if constexpr(std::is_rvalue_reference_v<decltype(c)>)
    {
      std::move(std::begin(c), std::end(c), std::back_inserter(*this));
    }
    else
    {
      std::copy(std::begin(c), std::end(c), std::back_inserter(*this));
    }

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

  // CAP = N - 1, N = CAP + 1
  static constexpr size_type capacity() noexcept { return CAP; }
  static constexpr size_type max_size() noexcept { return PTRDIFF_MAX; }

  //
  constexpr bool empty() const noexcept { return f_ == l_; }
  constexpr bool full() const noexcept { return next_(l_) == f_; }

  constexpr size_type size() const noexcept { return distance_(f_, l_); }

  //
  constexpr auto& operator[](size_type const i) noexcept
  {
    return *next_(f_, i);
  }

  constexpr auto const& operator[](size_type const i) const noexcept
  {
    return *next_(f_, i);
  }

  constexpr auto& at(size_type const i) noexcept { return (*this)[i]; }
  constexpr auto& at(size_type const i) const noexcept { return (*this)[i]; }

  constexpr auto& back() noexcept { return *prev_(l_); }
  constexpr auto const& back() const noexcept { return *prev_(l_); }

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
    noexcept(noexcept(clear(), std::copy(i, j, std::back_inserter(*this))))
  {
    clear(); std::copy(i, j, std::back_inserter(*this));
  }

  constexpr void assign(std::initializer_list<value_type> l)
    noexcept(noexcept(assign(l.begin(), l.end())))
  {
    assign(l.begin(), l.end());
  }

  //
  constexpr void clear() noexcept { l_ = f_; }
  constexpr void reset() noexcept { l_ = f_ = a_; }
  constexpr void resize(size_type const n) noexcept { l_ = next_(f_, n); }

  // emplacing is a bad idea in this container, avoid if possible
  constexpr void emplace_back(auto&& a)
    noexcept(noexcept(push_back(std::forward<decltype(a)>(a))))
    requires(std::is_assignable_v<value_type&, decltype(a)>)
  {
    push_back(std::forward<decltype(a)>(a));
  }

  constexpr void emplace_back(auto&& ...a)
    noexcept(noexcept(push_back(std::declval<T>())))
    requires(std::is_constructible_v<T, decltype(a)...>)
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
    requires(std::is_constructible_v<T, decltype(a)...>)
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
    requires(std::is_constructible_v<T, decltype(a)...>)
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
    noexcept(noexcept(std::move(i, i, i), std::move_backward(i, i, i)))
  {
    if (iterator const j(this, i.n()), nxt(std::next(j));
      distance_(nxt.n(), l_) <= distance_(f_, j.n()))
    {
      l_ = std::move(nxt, end(), j).n();

      return j; // *nxt is moved to *j
    }
    else
    {
      f_ = std::move_backward(begin(), j, nxt).n();

      return nxt; // *nxt is not moved
    }
  }

  constexpr iterator erase(const_iterator a, const_iterator const b)
    noexcept(noexcept(erase(a)))
  {
    for (auto n(distance_(a.n(), b.n())); n--;) a = erase(a);

    return {this, a.n()};
  }

  //
  template <int = 0>
  constexpr iterator insert(const_iterator const i, auto&& v)
    noexcept(noexcept(std::move(i, i, i), std::move_backward(i, i, i)))
    requires(std::is_assignable_v<value_type&, decltype(v)>)
  {
    if (full()) [[unlikely]] pop_front();

    //
    iterator j(this, i.n());

    if (distance_(f_, i.n()) <= distance_(i.n(), l_))
    {
      auto const f(f_);
      f_ = prev_(f);

      j = std::move({this, f}, j, begin()); // [f, j) is moved
    }
    else
    {
      auto const l(l_);
      l_ = next_(l);

      std::move_backward(j, {this, l}, end()); // [j, l) is moved
    }

    //
    *j = std::forward<decltype(v)>(v);
    return j;
  }

  constexpr auto insert(const_iterator const i, value_type v)
    noexcept(noexcept(insert<0>(i, std::move(v))))
  {
    return insert<0>(i, std::move(v));
  }

  template <int = 0>
  constexpr iterator insert(multi_t, const_iterator i, auto&& ...a)
    noexcept(noexcept((insert(i, std::forward<decltype(a)>(a)), ...)))
    requires(requires{(insert(i, std::forward<decltype(a)>(a)), ...);})
  {
    (++(i = insert(i, std::forward<decltype(a)>(a))), ...);

    return {this, prev(i.n(), sizeof...(a))};
  }

  constexpr auto insert(multi_t, const_iterator const i, value_type a)
    noexcept(noexcept(insert<0>(multi_t{}, i, std::move(a))))
  {
    return insert<0>(multi_t{}, i, std::move(a));
  }

  template <int = 0>
  constexpr iterator insert(const_iterator i, size_type const count,
    auto const& v) noexcept(noexcept(insert(i, v)))
    requires(std::is_assignable_v<value_type&, decltype(v)>)
  {
    for (auto n(count); n--;) ++(i = insert(i, v));

    return {this, prev(i.n(), count)};
  }

  constexpr auto insert(const_iterator const i, size_type const count,
    value_type const v)
    noexcept(noexcept(insert<0>(i, count, v)))
  {
    return insert<0>(i, count, v);
  }

  constexpr iterator insert(const_iterator i,
    std::input_iterator auto const j, decltype(j) k)
    noexcept(noexcept(insert(i, *j)))
  {
    size_type n{};

    std::for_each(
      j,
      k,
      [&](auto&& v)
        noexcept(noexcept(insert(i, std::forward<decltype(v)>(v))))
      {
        ++n;
        ++(i = insert(i, std::forward<decltype(v)>(v)));
      }
    );

    return {this, prev_(i.n(), n)};
  }

  //
  constexpr void pop_back() noexcept { l_ = prev_(l_); }
  constexpr void pop_back(size_type const n) noexcept { l_ = prev_(l_, n); }
  constexpr void pop_front() noexcept { f_ = next_(f_); }
  constexpr void pop_front(size_type const n) noexcept { f_ = next_(f_, n); }

  //
  template <int = 0>
  constexpr void push_back(auto&& a)
    noexcept(std::is_nothrow_assignable_v<value_type&, decltype(a)>)
    requires(std::is_assignable_v<value_type&, decltype(a)>)
  {
    *l_ = std::forward<decltype(a)>(a);
    if ((l_ = next_(l_)) == f_) [[unlikely]] pop_front();
  }

  constexpr void push_back(auto&& ...v)
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
    *(full() ? f_ : f_ = prev_(f_)) = std::forward<decltype(v)>(v);
  }

  constexpr void push_front(auto&& ...v)
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
  constexpr void split(auto&& g) const
    noexcept(noexcept(g(f_, f_)))
  { // split the [f_, l_) range into 1 or 2 contiguous ranges
    l_ < f_ ? // f_ > l_ >= a_, f_ > a
      g(f_, decltype(f_)(&a_[N])), g(decltype(f_)(a_), l_) :
      g(f_, l_);
  }

  //
  constexpr void swap(array& o)
    noexcept(noexcept(std::swap(*this, o)))
    requires(MEMBER == M)
  {
    std::swap(*this, o);
  }

  constexpr void swap(array& o) noexcept
    requires((NEW == M) || (USER == M))
  {
    assign_(f_, l_, a_, o.f_, o.l_, o.a_)(o.f_, o.l_, o.a_, f_, l_, a_);
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
constexpr void swap(array<T, S, M>& l, decltype(l) r) noexcept { l.swap(r); }

}

#endif // CA_ARRAY_HPP
