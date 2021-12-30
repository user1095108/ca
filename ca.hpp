#ifndef CA_CA_HPP
# define CA_CA_HPP
# pragma once

#include <algorithm> // inplace_merge()
#include <limits>

#include "caiterator.hpp"

namespace ca
{

enum Method { MEMBER, NEW, USER };

template <typename T, std::size_t N, enum Method M = MEMBER>
class array
{
  static_assert(N > 1);

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

  std::conditional_t<MEMBER == M, T[N], T*> a_;

  static constexpr auto next(auto const a, auto const p) noexcept
  {
    return p == &a[N - 1] ? a : p + 1;
  }

  static constexpr auto prev(auto const a, auto const p) noexcept
  {
    return p == a ? &a[N - 1] : p - 1;
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
  constexpr array()
    noexcept(
      ((MEMBER == M) && std::is_nothrow_default_constructible_v<T[N]>) ||
      ((NEW == M) && noexcept(new T[N]))
    )
    requires((MEMBER == M) || (NEW == M))
  {
    if constexpr(NEW == M) a_ = new T[N];
    first_ = last_ = a_;
  }

  constexpr explicit array(T* const a) noexcept requires(USER == M):
    a_(first_ = last_ = a)
  {
  }

  constexpr ~array()
    noexcept(
      ((MEMBER == M) && std::is_nothrow_destructible_v<T[N]>) ||
      ((NEW == M) && noexcept(new T[N])) ||
      (USER == M)
    )
  {
    if constexpr(NEW == M) delete [] a_;
  }

  //
  constexpr array(array const& o)
    noexcept(noexcept(*this = o))
    requires(std::is_copy_assignable_v<value_type>)
  {
    *this = o;
  }

  constexpr array(array&& o)
    noexcept(noexcept(*this = std::move(o)))
    requires(std::is_move_assignable_v<value_type>)
  {
    *this = std::move(o);
  }

  // self-assign neglected
  constexpr array& operator=(array const& o)
    noexcept(std::is_nothrow_copy_assignable_v<value_type>)
    requires(std::is_copy_assignable_v<value_type>)
  {
    first_ = &a_[o.first - o.a_]; last_ = &a_[o.last_ - o.a_];
    std::copy(o.cbegin(), o.cend(), begin());

    return *this;
  }

  constexpr array& operator=(array&& o)
    noexcept(std::is_nothrow_move_assignable_v<value_type>)
    requires(std::is_move_assignable_v<value_type>)
  {
    if constexpr(MEMBER == M)
    {
      first_ = &a_[o.first - o.a_]; last_ = &a_[o.last_ - o.a_];
      std::move(o.begin(), o.end(), begin());
      o.clear();
    }
    else
    {
      first_ = o.first_; last_ = o.last_;
      o.first_ = o.last_ = a_;
      std::swap(a_, o.a_);
    }

    return *this;
  }

  //
  friend constexpr bool operator==(array const& l, array const& r) noexcept
  {
    return std::equal(l.begin(), l.end(), r.begin(), r.end());
  }

  friend constexpr auto operator<=>(array const& l, array const& r) noexcept
  {
    return std::lexicographical_compare_three_way(
      l.begin(), l.end(), r.begin(), r.end()
    );
  }

  friend bool operator!=(array const&, array const&) = default;
  friend bool operator<(array const&, array const&) = default;
  friend bool operator<=(array const&, array const&) = default;
  friend bool operator>(array const&, array const&) = default;
  friend bool operator>=(array const&, array const&) = default;

  //
  constexpr auto& operator=(std::initializer_list<value_type> const l)
    noexcept(noexcept(assign(l)))
    requires(std::is_copy_constructible_v<value_type>)
  {
    assign(l);

    return *this;
  }

  // iterators
  constexpr iterator begin() noexcept { return {this, first_}; }
  constexpr iterator end() noexcept { return {this, last_}; }

  constexpr const_iterator begin() const noexcept { return {this, first_}; }
  constexpr const_iterator end() const noexcept { return {this, last_}; }

  constexpr const_iterator cbegin() const noexcept { return {this, first_}; }
  constexpr const_iterator cend() const noexcept { return {this, last_}; }

  // reverse iterators
  constexpr reverse_iterator rbegin() noexcept
  {
    return reverse_iterator{iterator(this, last_)};
  }

  constexpr reverse_iterator rend() noexcept
  {
    return reverse_iterator{iterator(this, first_)};
  }

  // const reverse iterators
  constexpr const_reverse_iterator crbegin() const noexcept
  {
    return const_reverse_iterator{const_iterator(this, last_)};
  }

  constexpr const_reverse_iterator crend() const noexcept
  {
    return const_reverse_iterator{const_iterator{this, first_}};
  }

  //
  static constexpr size_type capacity() noexcept { return N - 1; }

  static constexpr size_type max_size() noexcept
  {
    return std::numeric_limits<difference_type>::max();
  }

  constexpr void clear() noexcept { first_ = last_; }
  constexpr bool empty() const noexcept { return first_ == last_; }
  constexpr bool full() const noexcept { return next(a_, last_) == first_; }

  constexpr size_type size() const noexcept
  {
    auto const n(last_ - first_);
    return n < 0 ? N + n : n;
  }

  //
  constexpr auto& operator[](size_type const i) noexcept
  {
    return *std::next(begin(), i);
  }

  constexpr auto const& operator[](size_type const i) const noexcept
  {
    return *std::next(begin(), i);
  }

  //
  constexpr const_reference at(size_type const i) noexcept
  {
    return *std::next(cbegin(), i);
  }

  constexpr const_reference at(size_type const i) const noexcept
  {
    return *std::next(cbegin(), i);
  }

  constexpr auto& back() noexcept { return *prev(a_, last_); }
  constexpr auto const& back() const noexcept { return *prev(a_, last_); }

  constexpr auto& front() noexcept { return *first_; }
  constexpr auto const& front() const noexcept { return *first_; }

  //
  constexpr void assign(std::initializer_list<value_type> const l)
    noexcept(std::is_nothrow_copy_assignable_v<value_type>)
    requires(std::is_copy_assignable_v<value_type>)
  {
    assign(l.begin(), l.end());
  }

  constexpr void assign(std::input_iterator auto const i, decltype(i) j)
    noexcept(std::is_nothrow_copy_assignable_v<value_type>)
    requires(std::is_copy_assignable_v<decltype(*i)>)
  {
    clear();
    std::copy(i, j, std::back_inserter(*this));
  }

  //
  constexpr iterator erase(const_iterator const i)
    noexcept(std::is_nothrow_move_assignable_v<value_type>)
  {
    if (iterator const j(this, i.n()), nxt(std::next(j)); end() == nxt)
    {
      return {this, last_ = prev(a_, last_)};
    }
    else if (size_type(std::distance(begin(), j)) <= size() / 2)
    {
      first_ = std::move_backward(begin(), j, nxt).n();
      return nxt;
    }
    else
    {
      last_ = std::move(nxt, end(), j).n();
      return j;
    }
  }

  constexpr iterator erase(const_iterator a, const_iterator const b)
    noexcept(noexcept(erase(a)))
  {
    iterator i(b);

    for (; a != b; i = erase(a), a = i);

    return i;
  }

  constexpr iterator erase(std::initializer_list<const_iterator> const l)
    noexcept(noexcept(erase(cbegin())))
  {
    iterator r;

    std::for_each(l.begin(), l.end(), [&](auto const i) { r = erase(i); });

    return r;
  }

  //
  constexpr iterator insert(const_iterator const i, auto&& v)
    noexcept(std::is_nothrow_assignable_v<value_type&, decltype(v)>)
    requires(std::is_assignable_v<value_type&, decltype(v)>)
  {
    if (full()) pop_front();

    //
    T* n;

    if (iterator const j(this, i.n()); end() == j)
    {
      last_ = next(a_, n = last_);
    }
    else if (size_type(std::distance(begin(), j)) <= size() / 2)
    {
      auto const f(first_);
      first_ = prev(a_, f);

      n = std::move({this, f}, j, begin()).n();
    }
    else
    {
      n = j.n();

      auto const l(last_);
      last_ = next(a_, l);

      std::move_backward(j, {this, l}, end());
    }

    //
    *n = std::forward<decltype(v)>(v);

    return {this, n};
  }

  //
  constexpr void pop_back() noexcept { last_ = prev(a_, last_); }
  constexpr void pop_front() noexcept { first_ = next(a_, first_); }

  constexpr void push_back(auto&& v)
    noexcept(std::is_nothrow_assignable_v<value_type&, decltype(v)>)
    requires(std::is_assignable_v<value_type&, decltype(v)>)
  {
    auto const l(last_);
    *l = std::forward<decltype(v)>(v);
    if ((last_ = next(a_, l)) == first_) pop_front();
  }

  constexpr void push_front(auto&& v)
    noexcept(std::is_nothrow_assignable_v<value_type&, decltype(v)>)
    requires(std::is_assignable_v<value_type&, decltype(v)>)
  {
    *(full() ? first_ : first_ = prev(a_, first_)) =
      std::forward<decltype(v)>(v);
  }

  //
  void sort() { sort(std::less<value_type>()); }
  void sort(auto cmp) { sort(begin(), end(), size(), cmp); }

  constexpr void swap(array& o) noexcept requires((NEW == M) || (USER == M))
  {
    std::swap(first_, o.first_);
    std::swap(last_, o.last_);
    std::swap(a_, o.a_);
  }

  //
  friend constexpr auto erase(array& c, auto const& k)
    noexcept(std::is_nothrow_move_assignable_v<T>)
  {
    return erase_if(c, [&](auto&& v) noexcept{return std::equal_to()(v, k);});
  }

  friend constexpr auto erase_if(array& c, auto pred)
    noexcept(std::is_nothrow_move_assignable_v<T>)
  {
    size_type r{};

    for (auto i(c.begin()); c.end() != i;)
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

  friend constexpr void swap(array& lhs, decltype(lhs) rhs) noexcept
    requires((NEW == M) || (USER == M))
  {
    lhs.swap(rhs);
  }
};

}

#endif // CA_CA_HPP
