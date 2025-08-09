#ifndef DQ_ARRAY_HPP
# define DQ_ARRAY_HPP
# pragma once

#include <cstdint> // PTRDIFF_MAX
#include <algorithm> // std::move()
#include <compare> // std::three_way_comparable
#include <execution> // std::execution
#include <initializer_list> // std::initializer_list
#include <ranges>

#include "arrayiterator.hpp"

namespace dq
{

struct from_range_t { explicit from_range_t() = default; };
inline constexpr from_range_t from_range{};

struct multi_t { explicit multi_t() = default; };
inline constexpr multi_t multi{};

template <typename T, std::size_t CAP, enum Method M = MEMBER,
  auto E = std::execution::unseq>
requires(
  !std::is_reference_v<T> &&
  !std::is_const_v<T> &&
  std::is_default_constructible_v<T> &&
  (CAP > 0) && (CAP < PTRDIFF_MAX) &&
  (std::is_copy_assignable_v<T> || std::is_move_assignable_v<T>)
) // N = CAP + 1 <= PTRDIFF_MAX
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

  enum {ca_array_tag};

//private:
  enum : size_type { N = CAP + 1 };

  T* f_, *l_; // pointer to first and last elements of element array
  std::conditional_t<MEMBER == M, T[N], T*> a_; // element array

  constexpr auto next_(auto const p) const noexcept
  {
    return p == std::addressof(a_[N - 1]) ?
      decltype(p)(a_) : p + difference_type(1);
  }

  constexpr auto prev_(auto const p) const noexcept
  {
    return p == a_ ?
      decltype(p)(std::addressof(a_[N - 1])) : p - difference_type(1);
  }

  constexpr auto next_(auto const p, difference_type const n) const noexcept
  { // 0 <= n < N
    assert((n >= 0) && (n < difference_type(N)));
    //auto const u(difference_type(N) - n); return p - a_ < u ? p + n : p - u;
    return std::addressof(a_[N]) - p > n ?
      p + n : p - (difference_type(N) - n);
  }

  constexpr auto prev_(auto const p, difference_type const n) const noexcept
  { // 0 <= n < N
    assert((n >= 0) && (n < difference_type(N)));
    return p - a_ < n ? p + (difference_type(N) - n) : p - n;
  }

  constexpr auto adv_(auto const p, difference_type const n) const noexcept
  { // -N < n < N
    assert(-difference_type(N) < n); assert(n < difference_type(N));
    return std::addressof(a_[N]) - p <= n ? // p + n >= &a_[N]
      p - (difference_type(N) - n) :
      a_ - p > n ? // p + n < a_
      p + (difference_type(N) + n) :
      p + n;
  }

  static constexpr auto distance_(auto const a, decltype(a) b) noexcept
  { // N = CAP + 1 <= PTRDIFF_MAX
    auto const n(b - a);
    return n < difference_type{} ? difference_type(N) + n : n;
  }

public:
  constexpr array()
    noexcept(std::is_nothrow_default_constructible_v<T[N]>)
    requires(MEMBER == M):
    f_(l_ = a_)
  {
    if (std::is_constant_evaluated()) for (auto& a: a_) a = T{};
  }

  constexpr array()
    noexcept(noexcept(new T[N], T{}))
    requires(NEW == M)
  {
    f_ = l_ = a_ = new T[N];
    if (std::is_constant_evaluated()) std::ranges::generate(
      *this, []() noexcept(noexcept(T{})) { return T{}; });
  }

  constexpr array(array const& o)
    noexcept(noexcept(array(o.begin(), o.end())))
    requires(std::is_copy_assignable_v<value_type>):
    array(o.begin(), o.end())
  {
  }

  constexpr array(array&& o)
    noexcept(noexcept(array(), o.clear(),
      std::move(E, o.begin(), o.end(), std::back_inserter(*this))))
    requires(MEMBER == M):
    array()
  {
    if (std::is_constant_evaluated())
      std::move(o.begin(), o.end(), std::back_inserter(*this));
    else
      std::move(E, o.begin(), o.end(), std::back_inserter(*this));

    o.clear();
  }

  constexpr array(array&& o) noexcept(noexcept(array())) requires(NEW == M):
    array()
  { // swap & reset
    detail::assign(f_, l_, a_, o.f_, o.l_, o.a_)
      (o.f_, o.l_, o.a_, a_, a_, a_);
  }

  constexpr array(multi_t, auto&& ...a)
    noexcept(noexcept(push_back(std::forward<decltype(a)>(a)...)))
    requires(!!sizeof...(a)):
    array()
  {
    push_back(std::forward<decltype(a)>(a)...);
  }

  constexpr array(std::input_iterator auto const i, decltype(i) j)
    noexcept(noexcept(std::copy(E, i, j, std::back_inserter(*this)))):
    array()
  {
    if (std::is_constant_evaluated())
      std::copy(i, j, std::back_inserter(*this));
    else
      std::copy(E, i, j, std::back_inserter(*this));
  }

  constexpr array(std::initializer_list<value_type> l)
    noexcept(noexcept(array(l.begin(), l.end()))):
    array(l.begin(), l.end())
  {
  }

  template <typename V = value_type>
  constexpr explicit array(size_type const c, V const& v = V{}, int = 0)
    noexcept(noexcept(array(), std::fill(E, f_, l_, v))):
    array()
  {
    if (resize(c); std::is_constant_evaluated())
      std::fill(f_, l_, v);
    else
      std::fill(E, f_, l_, v);
  }

  constexpr explicit array(size_type const c, value_type const v)
    noexcept(noexcept(array(c, v, 0))):
    array(c, v, 0)
  {
  }

  constexpr explicit array(std::ranges::input_range auto&& rg)
    noexcept(noexcept(array(std::ranges::begin(rg), std::ranges::end(rg))))
    requires(!std::is_same_v<std::remove_cvref_t<decltype(rg)>, array>):
    array(std::ranges::begin(rg), std::ranges::end(rg))
  {
  }

  constexpr array(from_range_t, std::ranges::input_range auto&& rg)
    noexcept(noexcept(array(std::ranges::begin(rg), std::ranges::end(rg)))):
    array(std::ranges::begin(rg), std::ranges::end(rg))
  {
  }

  ~array() requires(NEW != M) = default;

  constexpr ~array() noexcept(noexcept(delete [] a_)) requires(NEW == M)
  {
    delete [] a_;
  }

  //
  constexpr array& operator=(array const& o)
    noexcept(noexcept(clear(),
      std::copy(E, o.begin(), o.end(), std::back_inserter(*this))))
    requires(std::is_copy_assignable_v<value_type>)
  {
    if (this != &o)
    {
      clear();

      if (std::is_constant_evaluated())
        std::copy(o.begin(), o.end(), std::back_inserter(*this));
      else
        std::copy(E, o.begin(), o.end(), std::back_inserter(*this));
    }

    return *this;
  }

  constexpr array& operator=(array&& o)
    noexcept(noexcept(clear(), o.clear(),
      std::move(E, o.begin(), o.end(), std::back_inserter(*this))))
    requires(MEMBER == M)
  {
    if (this != &o)
    {
      clear();

      if (std::is_constant_evaluated())
        std::move(o.begin(), o.end(), std::back_inserter(*this));
      else
        std::move(E, o.begin(), o.end(), std::back_inserter(*this));

      o.clear();
    }

    return *this;
  }

  constexpr array& operator=(array&& o) noexcept requires(NEW == M)
  { // swap & reset
    if (this != &o)
      detail::assign(f_, l_, a_, o.f_, o.l_, o.a_)
        (o.f_, o.l_, o.a_, a_, a_, a_);

    return *this;
  }

  constexpr array& operator=(std::initializer_list<value_type> l)
    noexcept(noexcept(assign(l)))
  {
    assign(l); return *this;
  }

  constexpr array& operator=(std::ranges::input_range auto&& rg)
    noexcept(noexcept(assign_range(std::forward<decltype(rg)>(rg))))
    requires(!std::is_same_v<std::remove_cvref_t<decltype(rg)>, array>)
  {
    assign_range(std::forward<decltype(rg)>(rg)); return *this;
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

  constexpr auto cbegin() const noexcept { return begin(); }
  constexpr auto cend() const noexcept { return end(); }

  // reverse iterators
  constexpr reverse_iterator rbegin() noexcept
  {
    return reverse_iterator{iterator(this, l_)};
  }

  constexpr reverse_iterator rend() noexcept
  {
    return reverse_iterator{iterator(this, f_)};
  }

  constexpr const_reverse_iterator rbegin() const noexcept
  {
    return const_reverse_iterator{const_iterator(this, l_)};
  }

  constexpr const_reverse_iterator rend() const noexcept
  {
    return const_reverse_iterator{const_iterator{this, f_}};
  }

  constexpr auto crbegin() const noexcept { return rbegin(); }
  constexpr auto crend() const noexcept { return rend(); }

  // N = CAP + 1 <= PTRDIFF_MAX
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
    clear(); while (count) --count, push_back(v);
  }

  constexpr void assign(size_type const count, value_type const v)
    noexcept(noexcept(assign<0>(count, v)))
  {
    assign<0>(count, v);
  }

  constexpr void assign(std::input_iterator auto const i, decltype(i) j)
    noexcept(noexcept(clear(), std::copy(E, i, j, std::back_inserter(*this))))
  {
    clear();

    if (std::is_constant_evaluated())
      std::copy(i, j, std::back_inserter(*this));
    else
      std::copy(E, i, j, std::back_inserter(*this));
  }

  constexpr void assign(std::initializer_list<value_type> l)
    noexcept(noexcept(assign(l.begin(), l.end())))
  {
    assign(l.begin(), l.end());
  }

  constexpr void assign_range(std::ranges::input_range auto&& rg)
    noexcept(noexcept(assign(std::ranges::begin(rg), std::ranges::end(rg))))
    requires(!std::is_same_v<std::remove_cvref_t<decltype(rg)>, array>)
  {
    assign(std::ranges::begin(rg), std::ranges::end(rg));
  }

  constexpr void assign_range(std::ranges::input_range auto&& rg)
    noexcept(noexcept(assign(std::ranges::begin(rg), std::ranges::end(rg))))
    requires(std::is_same_v<std::remove_cvref_t<decltype(rg)>, array>)
  {
    if (this != &rg) assign(std::ranges::begin(rg), std::ranges::end(rg));
  }

  //
  constexpr void clear() noexcept { l_ = f_; }
  constexpr void resize(size_type const c) noexcept { l_ = next_(f_, c); }

  template <int = 0>
  constexpr void resize(size_type const c, auto const& a)
    noexcept(noexcept(push_back(a), resize(c)))
    requires(std::is_assignable_v<value_type&, decltype(a)>)
  {
    for (auto sz(size()); c > sz; ++sz, push_back(a));
    resize(c);
  }

  constexpr void resize(size_type const c, value_type const a)
    noexcept(noexcept(resize<0>(c, a)))
  {
    resize<0>(c, a);
  }

  // emplacing is a bad idea in this container, avoid if possible
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
    noexcept(noexcept(std::move(E, i, i, i)))
  {
    iterator const ii{this, i.n_}, jj{this, next_(i.n_)};

    return distance_(f_, ii.n_) <= distance_(jj.n_, l_) ?
      //f_ = std::move_backward(begin(), ii, jj).n_, jj:
      f_ = std::move(E, reverse_iterator(ii), rend(),
        reverse_iterator(jj)).base().n_, jj:
      (l_ = std::move(E, jj, end(), ii).n_, ii);
  }

  constexpr iterator erase(const_iterator const i, const_iterator const j)
    noexcept(noexcept(std::move(E, i, i, i)))
  {
    if (iterator const ii{this, i.n_}; i == j) [[unlikely]]
    {
      return ii;
    }
    else [[likely]]
    {
      decltype(ii) jj{this, j.n_};

      return distance_(f_, ii.n_) <= distance_(jj.n_, l_) ?
        f_ = std::move(E, reverse_iterator(ii), rend(),
          reverse_iterator(jj)).base().n_, jj:
        (l_ = std::move(E, jj, end(), ii).n_, ii);
    }
  }

  //
  template <int = 0>
  constexpr iterator insert(const_iterator const i, auto&& a)
    noexcept(noexcept(std::move(E, i, i, i)))
    requires(std::is_assignable_v<value_type&, decltype(a)>)
  {
    if (full()) [[unlikely]] pop_front();

    //
    iterator j{this};

    if (distance_(f_, i.n_) <= distance_(i.n_, l_))
    { // [f, i) is moved backwards
      auto const f(f_); f_ = prev_(f);

      j.n_ = std::move(E, {this, f}, i, begin()).n_;
    }
    else
    { // [j, l) is moved forwards
      j.n_ = i.n_; auto const l(l_); l_ = next_(l); 

      //std::move_backward(j, {this, l}, end());
      std::move(E,
        const_reverse_iterator(const_iterator{this, l}),
        const_reverse_iterator(i), rbegin());
    }

    //
    *j = std::forward<decltype(a)>(a); return j;
  }

  constexpr auto insert(const_iterator const i, value_type a)
    noexcept(noexcept(insert<0>(i, std::move(a))))
  {
    return insert<0>(i, std::move(a));
  }

  template <int = 0>
  constexpr iterator insert(multi_t, const_iterator i, auto&& ...a)
    noexcept(noexcept((insert(i, std::forward<decltype(a)>(a)), ...)))
    requires(sizeof...(a) > 1)
  {
    (++(i = insert(i, std::forward<decltype(a)>(a))), ...);

    return {this, prev(i.n_, sizeof...(a))};
  }

  constexpr auto insert(multi_t, const_iterator const i, value_type a)
    noexcept(noexcept(insert<0>(i, std::move(a))))
  {
    return insert<0>(i, std::move(a));
  }

  template <int = 0>
  constexpr iterator insert(const_iterator i, size_type const count,
    auto const& a)
    noexcept(noexcept(insert(i, a)))
    requires(std::is_assignable_v<value_type&, decltype(a)>)
  {
    for (auto n(count); n; --n, ++(i = insert(i, a)));

    return {this, prev(i.n_, count)};
  }

  constexpr auto insert(const_iterator const i, size_type const count,
    value_type const a)
    noexcept(noexcept(insert<0>(i, count, a)))
  {
    return insert<0>(i, count, a);
  }

  constexpr iterator insert(const_iterator i,
    std::input_iterator auto const j, decltype(j) k)
    noexcept(noexcept(insert(i, *j)))
  {
    size_type n{};

    std::for_each(
      j,
      k,
      [&](auto&& v) noexcept(noexcept(
        insert(i, std::forward<decltype(v)>(v))))
      {
        ++n, ++(i = insert(i, std::forward<decltype(v)>(v)));
      }
    );

    return {this, prev_(i.n_, n)};
  }

  constexpr auto insert(const_iterator const i,
    std::initializer_list<value_type> l)
    noexcept(noexcept(insert(i, l.begin(), l.end())))
  {
    return insert(i, l.begin(), l.end());
  }

  //
  constexpr auto insert_range(const_iterator const pos,
    std::ranges::input_range auto&& rg)
    noexcept(noexcept(insert(pos, std::ranges::begin(rg),
      std::ranges::end(rg))))
  {
    return insert(pos, std::ranges::begin(rg), std::ranges::end(rg));
  }

  constexpr void append_range(std::ranges::input_range auto&& rg)
    noexcept(noexcept(std::copy(E, std::ranges::begin(rg),
      std::ranges::end(rg), std::back_inserter(*this))))
  {
    if (std::is_constant_evaluated())
      std::copy(std::ranges::begin(rg), std::ranges::end(rg),
        std::back_inserter(*this));
    else
      std::copy(E, std::ranges::begin(rg), std::ranges::end(rg),
        std::back_inserter(*this));
  }

  constexpr void prepend_range(std::ranges::input_range auto&& rg)
    noexcept(noexcept(std::copy(E, std::ranges::rbegin(rg),
      std::ranges::rend(rg), std::front_inserter(*this))))
  {
    if (std::is_constant_evaluated())
      std::copy(std::ranges::rbegin(rg),
        std::ranges::rend(rg), std::front_inserter(*this));
    else
      std::copy(E, std::ranges::rbegin(rg),
        std::ranges::rend(rg), std::front_inserter(*this));
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

  constexpr void push_back(auto&& ...a)
    noexcept(noexcept((push_back<0>(std::forward<decltype(a)>(a)), ...)))
    requires(sizeof...(a) > 1)
  {
    (push_back<0>(std::forward<decltype(a)>(a)), ...);
  }

  constexpr void push_back(value_type a)
    noexcept(noexcept(push_back<0>(std::move(a))))
  {
    push_back<0>(std::move(a));
  }

  template <int = 0>
  constexpr void push_front(auto&& a)
    noexcept(std::is_nothrow_assignable_v<value_type&, decltype(a)>)
    requires(std::is_assignable_v<value_type&, decltype(a)>)
  { // pop_front() + push_front() = overwrite_front()
    *(full() ? f_ : f_ = prev_(f_)) = std::forward<decltype(a)>(a);
  }

  constexpr void push_front(auto&& ...a)
    noexcept(noexcept((push_front<0>(std::forward<decltype(a)>(a)), ...)))
    requires(sizeof...(a) > 1)
  {
    (push_front<0>(std::forward<decltype(a)>(a)), ...);
  }

  constexpr void push_front(value_type a)
    noexcept(noexcept(push_front<0>(std::move(a))))
  {
    push_front<0>(std::move(a));
  }

  //
  constexpr void swap(array& o)
    noexcept(noexcept(std::swap(*this, o)))
    requires(MEMBER == M)
  {
    std::swap(*this, o);
  }

  constexpr void swap(array& o) noexcept
    requires(NEW == M)
  { // swap state
    detail::assign(f_, l_, a_, o.f_, o.l_, o.a_)
      (o.f_, o.l_, o.a_, f_, l_, a_);
  }

  //
  constexpr auto append(T const* const p, size_type cnt) noexcept
  { // appends to container from a memory region
    cnt = std::min(cnt, capacity() - size());

    auto const nc(std::min(size_type(
      f_ <= l_ ? std::addressof(a_[N]) - l_ : f_ - l_ - 1), cnt)); // !!!

    if (std::is_constant_evaluated())
      std::copy_n(p, nc, l_), std::copy_n(p + nc, cnt - nc, a_);
    else
      std::copy_n(E, p, nc, l_), std::copy_n(E, p + nc, cnt - nc, a_);

    l_ = next_(l_, cnt);

    return cnt;
  }

  constexpr std::array<std::array<T*, 2>, 2> split() noexcept
  {
    using res_t = decltype(split());
    using pair_t = res_t::value_type;

    return f_ <= l_ ? res_t{pair_t{f_, l_}} :
      res_t{pair_t{f_, std::addressof(a_[N])}, pair_t{a_, l_}};
  }

  constexpr std::array<std::array<T const*, 2>, 2> split() const noexcept
  {
    using res_t = decltype(split());
    using pair_t = res_t::value_type;

    return f_ <= l_ ? res_t{pair_t{f_, l_}} :
      res_t{pair_t{f_, std::addressof(a_[N])}, pair_t{a_, l_}};
  }

  constexpr auto csplit() const noexcept { return split(); }
};

//////////////////////////////////////////////////////////////////////////////
template <typename T, auto S, auto M, auto E>
constexpr auto erase_if(array<T, S, M, E>& c, auto&& pred)
  noexcept(noexcept(c.erase(c.cbegin()), pred(*c.cbegin())))
{
  typename std::remove_reference_t<decltype(c)>::size_type r{};

  for (auto&& [i, j]: c.csplit())
  {
    if (i == j) break;

    while ((i != j) && (i != c.last()))
      pred(*i) ? ++r, i = std::addressof(*c.erase({&c, i})) : ++i;

    if (c.first() <= c.last()) break;
  }

  return r;
}

template <int = 0, typename T, auto S, auto M, auto E>
constexpr auto erase(array<T, S, M, E>& c, auto const& ...k)
  noexcept(noexcept(c.erase({}), ((*c.cbegin() == k), ...)))
  requires(!!sizeof...(k))
{
  return erase_if(
      c,
      [&k...](auto& a) noexcept(noexcept(((a == k), ...)))
      {
        return ((a == k) || ...);
      }
    );
}

template <typename T, auto S, auto M, auto E>
constexpr auto erase(array<T, S, M, E>& c, T const k)
  noexcept(noexcept(erase<0>(c, k)))
{
  return erase<0>(c, k);
}

constexpr auto find_if(auto&& c, auto pred)
  noexcept(noexcept(pred(*c.cbegin()))) ->
  decltype(c.end())
  requires(requires{std::remove_cvref_t<decltype(c)>::ca_array_tag;})
{
  for (auto [i, j]: c.split())
  {
    if (i == j) break;

    for (; i < --j; ++i)
      if (pred(std::as_const(*i))) return {&c, i};
      else if (pred(std::as_const(*j))) return {&c, j};

    if ((i == j) && pred(std::as_const(*i))) return {&c, i}; // !!!
  }

  return c.end();
}

template <int = 0>
constexpr auto find(auto&& c, auto const& ...k)
  noexcept(noexcept(((*c.cbegin() == k), ...)))
  requires(!!sizeof...(k))
{
  return find_if(
      std::forward<decltype(c)>(c),
      [&k...](auto const& a) noexcept(noexcept(((a == k), ...)))
      {
        return ((a == k) || ...);
      }
    );
}

template <typename T, auto S, auto M, auto E>
constexpr auto find(array<T, S, M, E>& c, T const k)
  noexcept(noexcept(find<0>(c, k)))
{
  return find<0>(c, k);
}

template <typename T, auto S, auto M, auto E>
constexpr auto find(array<T, S, M, E> const& c, T const k)
  noexcept(noexcept(find<0>(c, k)))
{
  return find<0>(c, k);
}

//////////////////////////////////////////////////////////////////////////////
template <typename T1, auto S1, auto M1, auto E1,
  typename T2, auto S2, auto M2, auto E2>
constexpr bool operator==(array<T1, S1, M1, E1> const& l,
  array<T2, S2, M2, E2> const& r)
  noexcept(noexcept(std::equal(l.begin(), l.end(), r.begin(), r.end())))
{
  return std::equal(l.begin(), l.end(), r.begin(), r.end());
}

template <typename T1, auto S1, auto M1, auto E1,
  typename T2, auto S2, auto M2, auto E2>
constexpr auto operator<=>(array<T1, S1, M1, E1> const& l,
  array<T2, S2, M2, E2> const& r)
  noexcept(noexcept(std::lexicographical_compare_three_way(
    l.begin(), l.end(), r.begin(), r.end())))
{
  return std::lexicographical_compare_three_way(
    l.begin(), l.end(), r.begin(), r.end());
}

template <auto EX = std::execution::unseq, typename T, auto S, auto M, auto E>
constexpr void copy(array<T, S, M, E> const& a, T* p) noexcept
{ // copies from container to a memory region
  for (auto const [i, j]: a.split()) // !!!
  {
    if (i == j) break;

    if (std::is_constant_evaluated())
      std::copy(i, j, p);
    else
      std::copy(EX, i, j, p);

    p += j - i;
  }
}

template <auto EX = std::execution::unseq, typename T, auto S, auto M, auto E>
constexpr void copy(array<T, S, M, E> const& a, T* p,
  typename array<T, S, M, E>::size_type sz) noexcept
{ // copies from container to a memory region
  for (auto const [i, j]: a.split()) // !!!
  {
    if (i == j) break;

    auto const nc(std::min(decltype(sz)(j - i), sz));
    sz -= nc;
    if (std::is_constant_evaluated())
      std::copy_n(i, nc, p);
    else
      std::copy_n(EX, i, nc, p);
    p += nc;
  }
}

template <typename T, auto S, auto M>
constexpr void swap(array<T, S, M>& l, decltype(l) r) noexcept { l.swap(r); }

}

#endif // DQ_ARRAY_HPP
