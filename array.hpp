#ifndef CA_ARRAY_HPP
# define CA_ARRAY_HPP
# pragma once

#include <cstdint> // PTRDIFF_MAX
#include <algorithm> // std::move()
#include <compare> // std::three_way_comparable
#include <execution> // std::execution::seq
#include <initializer_list>
#include <ranges>

#include "arrayiterator.hpp"

namespace ca
{

struct from_range_t { explicit from_range_t() = default; };
inline constexpr from_range_t from_range{};

struct multi_t { explicit multi_t() = default; };
inline constexpr multi_t multi{};

template <typename T, std::size_t CAP, enum Method M = MEMBER>
requires(
  !std::is_reference_v<T> &&
  !std::is_const_v<T> &&
  std::is_default_constructible_v<T> &&
  (CAP > 0) && (CAP <= PTRDIFF_MAX) &&
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

  enum {ca_array_tag};

//private:
  enum : size_type { N = CAP + 1 };

  T* f_, *l_; // pointer to first and last elements of element array
  std::conditional_t<MEMBER == M, T[N], T*> a_; // element array

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
    requires(MEMBER == M)
  {
    if (std::is_constant_evaluated())
    {
      [&]<auto ...I>(std::index_sequence<I...>)
        noexcept(std::is_nothrow_move_assignable_v<T>)
      {
        ((a_[I] = T()), ...);
      }(std::make_index_sequence<N>());
    }

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
    noexcept(noexcept(
      o.clear(), std::move(o.begin(), o.end(), std::back_inserter(*this))))
    requires(MEMBER == M):
    array()
  {
    std::move(o.begin(), o.end(), std::back_inserter(*this));
    o.clear();
  }

  constexpr array(array&& o) noexcept requires(NEW == M):
    array()
  { // swap & o.reset()
    detail::assign(f_, l_, a_, o.f_, o.l_, o.a_)
      (o.f_, o.l_, o.a_, a_, a_, a_);
  }

  constexpr array(array&& o) noexcept requires(USER == M)
  {
    f_ = o.f_; l_ = o.l_; a_ = o.a_; // o.a_ stays unchanged
    o.clear();
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

  constexpr explicit array(size_type const c, auto const& v, int = 0)
    noexcept(noexcept(array(c), std::fill(f_, l_, v)))
    requires(std::is_assignable_v<value_type&, decltype(v)>):
    array(c)
  {
    std::fill(f_, l_, v);
  }

  constexpr explicit array(size_type const c, value_type const v)
    noexcept(noexcept(array(c, v, 0))):
    array(c, v, 0)
  {
  }

  constexpr array(from_range_t, std::ranges::input_range auto&& rg)
    noexcept(noexcept(array(std::ranges::begin(rg), std::ranges::end(rg)))):
    array(std::ranges::begin(rg), std::ranges::end(rg))
  {
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
    noexcept(noexcept(o.clear(),
      std::move(o.begin(), o.end(), std::back_inserter(*this))))
    requires(MEMBER == M)
  {
    clear(); std::move(o.begin(), o.end(), std::back_inserter(*this));
    o.clear();

    return *this;
  }

  constexpr array& operator=(array&& o) noexcept requires(NEW == M)
  { // swap & o.reset()
    detail::assign(f_, l_, a_, o.f_, o.l_, o.a_)
      (o.f_, o.l_, o.a_, a_, a_, a_);

    return *this;
  }

  constexpr array& operator=(array&& o) noexcept requires(USER == M)
  { // o.a_ stays unchanged
    f_ = o.f_; l_ = o.l_; a_ = o.a_; o.clear(); return *this;
  }

  constexpr array& operator=(std::initializer_list<value_type> l)
    noexcept(noexcept(assign(l)))
  {
    assign(l); return *this;
  }

  constexpr array& operator=(std::ranges::input_range auto&& rg)
    noexcept(noexcept(assign_range(std::forward<decltype(rg)>(rg))))
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

  constexpr void assign_range(std::ranges::input_range auto&& rg)
    noexcept(noexcept(assign(std::ranges::begin(rg), std::ranges::end(rg))))
  {
    assign(std::ranges::begin(rg), std::ranges::end(rg));
  }

  //
  constexpr void clear() noexcept { l_ = f_; }
  constexpr void reset() noexcept { l_ = f_ = a_; }
  constexpr void resize(size_type const c) noexcept { l_ = next_(f_, c); }

  template <int = 0>
  constexpr void resize(size_type const c, auto const& a)
    noexcept(noexcept(push_back(a)))
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
    iterator const ii{this, i.n_}, jj{this, next_(i.n_)};

    return distance_(f_, ii.n_) <= distance_(jj.n_, l_) ?
      f_ = std::move_backward(begin(), ii, jj).n_, jj:
      (l_ = std::move(jj, end(), ii).n_, ii);
  }

  constexpr iterator erase(const_iterator const i, const_iterator const j)
    noexcept(noexcept(std::move(i, i, i), std::move_backward(i, i, i)))
  {
    if (iterator const ii{this, i.n_}; i == j) [[unlikely]]
    {
      return ii;
    }
    else [[likely]]
    {
      decltype(ii) jj{this, j.n_};

      return distance_(f_, ii.n_) <= distance_(jj.n_, l_) ?
        f_ = std::move_backward(begin(), ii, jj).n_, jj :
        (l_ = std::move(jj, end(), ii).n_, ii);
    }
  }

  //
  template <int = 0>
  constexpr iterator insert(const_iterator const i, auto&& a)
    noexcept(noexcept(std::move(i, i, i), std::move_backward(i, i, i)))
    requires(std::is_assignable_v<value_type&, decltype(a)>)
  {
    if (full()) [[unlikely]] pop_front();

    //
    iterator j(this, i.n_);

    if (distance_(f_, i.n_) <= distance_(i.n_, l_))
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
    *j = std::forward<decltype(a)>(a);
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

    return {this, prev(i.n_, sizeof...(a))};
  }

  constexpr auto insert(multi_t, const_iterator const i, value_type a)
    noexcept(noexcept(insert<0>(multi_t{}, i, std::move(a))))
  {
    return insert<0>(multi, i, std::move(a));
  }

  template <int = 0>
  constexpr iterator insert(const_iterator i, size_type const count,
    auto const& v) noexcept(noexcept(insert(i, v)))
    requires(std::is_assignable_v<value_type&, decltype(v)>)
  {
    for (auto n(count); n; --n, ++(i = insert(i, v)));

    return {this, prev(i.n_, count)};
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
    noexcept(noexcept(
      insert(pos, std::ranges::begin(rg), std::ranges::end(rg))))
  {
    return insert(pos, std::ranges::begin(rg), std::ranges::end(rg));
  }

  constexpr void append_range(std::ranges::input_range auto&& rg)
    noexcept(noexcept(std::ranges::copy(rg, std::back_inserter(*this))))
  {
    std::ranges::copy(rg, std::back_inserter(*this));
  }

  constexpr void prepend_range(std::ranges::input_range auto&& rg)
    noexcept(noexcept(std::ranges::copy(std::ranges::views::reverse(rg),
      std::front_inserter(*this))))
  {
    std::ranges::copy(std::ranges::views::reverse(rg),
      std::front_inserter(*this));
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
    requires(requires{(push_back<0>(std::forward<decltype(a)>(a)), ...);})
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
    requires(requires{(push_front<0>(std::forward<decltype(a)>(a)), ...);})
  {
    (push_front<0>(std::forward<decltype(a)>(a)), ...);
  }

  constexpr void push_front(value_type a)
    noexcept(noexcept(push_front<0>(std::move(a))))
  {
    push_front<0>(std::move(a));
  }

  //
  template <auto exec = std::execution::seq>
  constexpr auto append(T const* const p, size_type cnt) noexcept
  { // appends to container from a memory region
    cnt = std::min(cnt, capacity() - size());

    auto const nc(std::min(f_ <= l_ ? size_type(-size_type(l_ - &a_[N])) :
      size_type(f_ - l_ - 1), cnt));

    std::copy(exec, p, p + nc, l_);
    std::copy(exec, p, cnt - nc + p, a_);

    l_ = next_(l_, cnt);

    return cnt;
  }

  template <auto exec = std::execution::seq>
  constexpr void copy(T* p) const noexcept
  { // copies from container to a memory region
    for (auto&& [i, j]: split())
    {
      if (!i) break;

      std::copy(exec, i, j, p);
      p += distance_(i, j);
    }
  }

  template <auto exec = std::execution::seq>
  constexpr void copy(T* p, size_type sz) const noexcept
  { // copies from container to a memory region
    for (auto&& [i, j]: split())
    {
      if (!i) break;

      auto const nc(std::min(distance_(i, j), sz));
      std::copy(exec, i, j, p, p + nc);
      p += nc;
      sz -= nc;
    }
  }

  constexpr std::array<std::array<T*, 2>, 2> split() noexcept
  {
    using pair_t = decltype(split())::value_type;

    if (auto const c(f_ <=> l_); c < 0) // f_ > l_ >= a_, f_ > a_
    {
      return {pair_t{f_, l_}};
    }
    else if (c > 0)
    {
      return {pair_t{f_, (T*)(&a_[N])}, pair_t{(T*)(a_), l_}};
    }
    else [[unlikely]]
    {
      return {};
    }
  }

  constexpr std::array<std::array<T const*, 2>, 2> split() const noexcept
  {
    using pair_t = decltype(split())::value_type;

    if (auto const c(f_ <=> l_); c < 0) // f_ > l_ >= a_, f_ > a_
    {
      return {pair_t{f_, l_}};
    }
    else if (c > 0)
    {
      return {pair_t{f_, (T const*)(&a_[N])}, pair_t{(T const*)(a_), l_}};
    }
    else [[unlikely]]
    {
      return {};
    }
  }

  auto csplit() const noexcept { return split(); }

  //
  constexpr void swap(array& o)
    noexcept(noexcept(std::swap(*this, o)))
    requires(MEMBER == M)
  {
    std::swap(*this, o);
  }

  constexpr void swap(array& o) noexcept
    requires((NEW == M) || (USER == M))
  { // swap state
    detail::assign(f_, l_, a_, o.f_, o.l_, o.a_)
      (o.f_, o.l_, o.a_, f_, l_, a_);
  }
};

//////////////////////////////////////////////////////////////////////////////
template <typename T, std::size_t S, enum Method M>
constexpr auto erase_if(array<T, S, M>& c, auto&& pred)
  noexcept(noexcept(c.erase(c.cbegin()), pred(*c.cbegin())))
{
  typename std::remove_reference_t<decltype(c)>::size_type r{};

  for (auto&& [i, j]: c.csplit())
  {
    if (!i) break;

    for (; (i != j) && (i != c.last());
      pred(*i) ? ++r, i = std::addressof(*c.erase({&c, i})) : ++i);

    if (c.first() <= c.last()) break;
  }

  return r;
}

template <int = 0, typename T, std::size_t S, enum Method M>
constexpr auto erase(array<T, S, M>& c, auto const& ...k)
  noexcept(noexcept(c.erase({}), ((*c.cbegin() == k), ...)))
  requires(requires{((*c.cbegin() == k), ...);})
{
  return erase_if(
      c,
      [&k...](auto& a) noexcept(noexcept(((a == k), ...)))
      {
        return ((a == k) || ...);
      }
    );
}

template <typename T, std::size_t S, enum Method M>
constexpr auto erase(array<T, S, M>& c, T const k)
  noexcept(noexcept(erase<0>(c, k)))
{
  return erase<0>(c, k);
}

constexpr auto find_if(auto&& c, auto pred)
  noexcept(noexcept(pred(*c.cbegin()))) ->
  decltype(c.end())
  requires(requires{std::remove_cvref_t<decltype(c)>::ca_array_tag;})
{
  for (auto&& [i, j]: c.split())
  {
    if (!i) break;

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
  requires(requires{((*c.cbegin() == k), ...);})
{
  return find_if(
      std::forward<decltype(c)>(c),
      [&k...](auto const& a) noexcept(noexcept(((a == k), ...)))
      {
        return ((a == k) || ...);
      }
    );
}

template <typename T, std::size_t S, enum Method M>
constexpr auto find(array<T, S, M>& c, T const k)
  noexcept(noexcept(find<0>(c, k)))
{
  return find<0>(c, k);
}

template <typename T, std::size_t S, enum Method M>
constexpr auto find(array<T, S, M> const& c, T const k)
  noexcept(noexcept(find<0>(c, k)))
{
  return find<0>(c, k);
}

//////////////////////////////////////////////////////////////////////////////
template <typename T1, std::size_t S1, enum Method M1,
  typename T2, std::size_t S2, enum Method M2>
constexpr bool operator==(array<T1, S1, M1> const& l,
  array<T2, S2, M2> const& r)
  noexcept(noexcept(std::equal(l.begin(), l.end(), r.begin(), r.end())))
{
  return std::equal(l.begin(), l.end(), r.begin(), r.end());
}

template <typename T1, std::size_t S1, enum Method M1,
  typename T2, std::size_t S2, enum Method M2>
constexpr auto operator<=>(array<T1, S1, M1> const& l,
  array<T2, S2, M2> const& r)
  noexcept(noexcept(std::lexicographical_compare_three_way(
    l.begin(), l.end(), r.begin(), r.end())))
{
  return std::lexicographical_compare_three_way(
    l.begin(), l.end(), r.begin(), r.end());
}

template <typename T, std::size_t S, enum Method M>
constexpr void swap(array<T, S, M>& l, decltype(l) r) noexcept { l.swap(r); }

}

#endif // CA_ARRAY_HPP
