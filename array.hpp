#ifndef CA_ARRAY_HPP
# define CA_ARRAY_HPP
# pragma once

#include <climits> // PTRDIFF_MAX
#include <algorithm> // inplace_merge()

#include "arrayiterator.hpp"

namespace ca
{

enum Method { MEMBER, NEW, USER };

template <typename T, std::size_t N, enum Method M = MEMBER>
class array
{
  static_assert(N > 1);
  static_assert(N - 1 <= PTRDIFF_MAX);

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
  T* f_, *l_; // pointer to first and last elements of element array
  std::conditional_t<MEMBER == M, T[N], T*> a_; // element array

  struct S
  {
    static constexpr auto next(auto const a, decltype(a) p) noexcept
    {
      return p == &a[N - 1] ? a : p + 1;
    }

    static constexpr auto prev(auto const a, decltype(a) p) noexcept
    {
      return p == a ? &a[N - 1] : p - 1;
    }

    //
    static void sort(auto const b, decltype(b) e,
      size_type const sz, auto&& c)
      noexcept(noexcept(
          std::inplace_merge(b, b, e, std::forward<decltype(c)>(c))
        )
      )
    {
      if (sz > 1)
      {
        auto const hsz(sz / 2);
        auto const m(std::next(b, hsz));

        sort(b, m, hsz, std::forward<decltype(c)>(c));
        sort(m, e, sz - hsz, std::forward<decltype(c)>(c));

        std::inplace_merge(b, m, e, std::forward<decltype(c)>(c));
      }
    }
  };

public:
  constexpr array()
    noexcept(
      ((MEMBER == M) && std::is_nothrow_default_constructible_v<T[N]>) ||
      ((NEW == M) && noexcept(new T[N]))
    )
    requires((MEMBER == M) || (NEW == M))
  {
    if constexpr(NEW == M) a_ = new T[N];
    f_ = l_ = a_;
  }

  constexpr explicit array(T* const a) noexcept requires(USER == M):
    a_(f_ = l_ = a)
  {
  }

  constexpr ~array()
    noexcept(
      ((MEMBER == M) && std::is_nothrow_destructible_v<T[N]>) ||
      ((NEW == M) && noexcept(delete [] std::declval<T*>())) ||
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
    requires(
      std::is_move_assignable_v<value_type> || (NEW == M) || (USER == M)
    )
  {
    *this = std::move(o);
  }

  // self-assign neglected
  constexpr array& operator=(array const& o)
    noexcept(std::is_nothrow_copy_assignable_v<value_type>)
    requires(std::is_copy_assignable_v<value_type>)
  {
    f_ = &a_[o.f_ - o.a_]; l_ = &a_[o.l_ - o.a_];
    std::copy(o.cbegin(), o.cend(), begin());

    return *this;
  }

  constexpr array& operator=(array&& o)
    noexcept(
      std::is_nothrow_move_assignable_v<value_type> ||
      (NEW == M) ||
      (USER == M)
    )
    requires(
      std::is_move_assignable_v<value_type> || (NEW == M) || (USER == M)
    )
  {
    if constexpr(MEMBER == M)
    {
      f_ = &a_[o.f_ - o.a_]; l_ = &a_[o.l_ - o.a_];
      std::move(o.begin(), o.end(), begin());
      o.clear();
    }
    else
    {
      f_ = o.f_; l_ = o.l_;
      o.f_ = o.l_ = a_;
      std::swap(a_, o.a_);
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
    noexcept(noexcept(
        std::lexicographical_compare_three_way(
          l.begin(), l.end(), r.begin(), r.end()
        )
      )
    )
  {
    return std::lexicographical_compare_three_way(
      l.begin(), l.end(), r.begin(), r.end()
    );
  }

  //
  constexpr auto& operator=(std::initializer_list<value_type> l)
    noexcept(noexcept(assign(l)))
    requires(std::is_copy_constructible_v<value_type>)
  {
    assign(l);

    return *this;
  }

  // iterators
  constexpr iterator begin() noexcept { return {a_, f_}; }
  constexpr iterator end() noexcept { return {a_, l_}; }

  constexpr const_iterator begin() const noexcept { return {a_, f_}; }
  constexpr const_iterator end() const noexcept { return {a_, l_}; }

  constexpr const_iterator cbegin() const noexcept { return {a_, f_}; }
  constexpr const_iterator cend() const noexcept { return {a_, l_}; }

  // reverse iterators
  constexpr reverse_iterator rbegin() noexcept
  {
    return reverse_iterator{iterator(a_, l_)};
  }

  constexpr reverse_iterator rend() noexcept
  {
    return reverse_iterator{iterator(a_, f_)};
  }

  // const reverse iterators
  constexpr const_reverse_iterator crbegin() const noexcept
  {
    return const_reverse_iterator{const_iterator(a_, l_)};
  }

  constexpr const_reverse_iterator crend() const noexcept
  {
    return const_reverse_iterator{const_iterator{a_, f_}};
  }

  //
  static constexpr size_type capacity() noexcept { return N - 1; }
  static constexpr size_type max_size() noexcept { return PTRDIFF_MAX; }

  //
  constexpr bool empty() const noexcept { return f_ == l_; }
  constexpr bool full() const noexcept { return S::next(a_, l_) == f_; }

  constexpr size_type size() const noexcept
  {
    auto const n(l_ - f_);

    return n < decltype(n){} ? N + n : n;
  }

  //
  constexpr auto& operator[](size_type i) noexcept
  {
    auto n(f_); for (; i; --i) n = S::next(a_, n); return *n;
  }

  constexpr auto const& operator[](size_type i) const noexcept
  {
    auto n(f_); for (; i; --i) n = S::next(a_, n); return *n;
  }

  constexpr auto& at(size_type const i) noexcept { return (*this)[i]; }
  constexpr auto& at(size_type const i) const noexcept { return (*this)[i]; }

  constexpr auto& back() noexcept { return *S::prev(a_, l_); }
  constexpr auto const& back() const noexcept { return *S::prev(a_, l_); }

  constexpr auto& front() noexcept { return *f_; }
  constexpr auto const& front() const noexcept { return *f_; }

  //
  constexpr void assign(std::initializer_list<value_type> l)
    noexcept(noexcept(assign(l.begin(), l.end())))
  {
    assign(l.begin(), l.end());
  }

  constexpr void assign(std::input_iterator auto const i, decltype(i) j)
    noexcept(std::is_nothrow_assignable_v<value_type&, decltype(*i)>)
    requires(std::is_assignable_v<T, decltype(*i)>)
  {
    clear();

    std::copy(i, j, std::back_inserter(*this));
  }

  //
  constexpr void clear() noexcept { f_ = l_; }

  //
  constexpr void emplace_back(auto&& ...a)
    noexcept(noexcept(push_back(std::declval<T>())))
  {
    push_back({std::forward<decltype(a)>(a)...});
  }

  constexpr void emplace_front(auto&& ...a)
    noexcept(noexcept(push_front(std::declval<T>())))
  {
    push_front({std::forward<decltype(a)>(a)...});
  }

  constexpr iterator emplace(const_iterator const i, auto&& ...a)
    noexcept(noexcept(emplace(std::declval<T>())))
  {
    return insert(i, {std::forward<decltype(a)>(a)...});
  }

  //
  constexpr iterator erase(const_iterator const i)
    noexcept(std::is_nothrow_move_assignable_v<value_type>)
  {
    if (iterator const j(a_, i.n()), nxt(std::next(j)); end() == nxt)
    {
      return {a_, l_ = S::prev(a_, l_)};
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
    iterator i(b); for (; a != b; a = i) i = erase(a); return i;
  }

  //
  constexpr iterator insert(const_iterator const i, auto&& v, char = {})
    noexcept(std::is_nothrow_assignable_v<value_type&, decltype(v)>)
    requires(std::is_assignable_v<value_type&, decltype(v)>)
  {
    if (full()) pop_front();

    //
    T* n;

    if (iterator const j(a_, i.n()); end() == j)
    {
      l_ = S::next(a_, n = l_);
    }
    else if (size_type(std::distance(begin(), j)) <= size() / 2)
    {
      auto const f(f_);
      f_ = S::prev(a_, f);

      n = std::move({a_, f}, j, begin()).n();
    }
    else
    {
      n = j.n();

      auto const l(l_);
      l_ = S::next(a_, l);

      std::move_backward(j, {a_, l}, end());
    }

    //
    *n = std::forward<decltype(v)>(v);

    return {a_, n};
  }

  constexpr iterator insert(const_iterator const i, value_type&& v)
    noexcept(noexcept(insert(i, std::move(v), {})))
  {
    return insert(i, std::move(v), {});
  }

  iterator insert(const_iterator i, size_type count, auto&& v)
    noexcept(noexcept(insert(i, std::declval<decltype(v)>())))
  {
    if (count)
    {
      auto const r(insert(i, v));
      i = std::next(r);

      for (--count; count; --count) i = std::next(insert(i, v));

      return r;
    }
    else
    {
      return {a_, i.n()};
    }
  }

  iterator insert(const_iterator i, size_type count, value_type&& v)
    noexcept(noexcept(insert(i, count, v)))
  {
    return insert(i, count, v);
  }

  iterator insert(const_iterator i,
    std::input_iterator auto const j, decltype(j) k)
    noexcept(noexcept(insert(i, *j)))
  {
    if (j == k)
    {
      return {a_, i.n()};
    }
    else
    {
      auto const r(emplace(i, *j));
      i = std::next(r);

      std::for_each(
        std::next(j),
        k,
        [&](auto&& v) noexcept(noexcept(insert(i, *j)))
        {
          i = std::next(insert(i, std::forward<decltype(v)>(v)));
        }
      );

      return r;
    }
  }

  //
  constexpr void pop_back() noexcept { l_ = S::prev(a_, l_); }
  constexpr void pop_front() noexcept { f_ = S::next(a_, f_); }

  //
  constexpr void push_back(value_type&& v)
    noexcept(std::is_nothrow_assignable_v<value_type&, decltype(v)>)
    requires(std::is_assignable_v<value_type&, decltype(v)>)
  {
    auto const l(l_);
    *l = std::move(v);
    if ((l_ = S::next(a_, l)) == f_) pop_front();
  }

  constexpr void push_back(auto&& v)
    noexcept(std::is_nothrow_assignable_v<value_type&, decltype(v)>)
    requires(std::is_assignable_v<value_type&, decltype(v)>)
  {
    auto const l(l_);
    *l = std::forward<decltype(v)>(v);
    if ((l_ = S::next(a_, l)) == f_) pop_front();
  }

  constexpr void push_front(value_type&& v)
    noexcept(std::is_nothrow_assignable_v<value_type&, decltype(v)>)
    requires(std::is_assignable_v<value_type&, decltype(v)>)
  {
    *(full() ? f_ : f_ = S::prev(a_, f_)) = std::move(v);
  }

  constexpr void push_front(auto&& v)
    noexcept(std::is_nothrow_assignable_v<value_type&, decltype(v)>)
    requires(std::is_assignable_v<value_type&, decltype(v)>)
  {
    *(full() ? f_ : f_ = S::prev(a_, f_)) = std::forward<decltype(v)>(v);
  }

  //
  void sort(auto cmp) noexcept(noexcept(S::sort(begin(), end(), size(), cmp)))
  {
    S::sort(begin(), end(), size(), cmp);
  }

  void sort() noexcept(noexcept(sort(std::less<value_type>())))
  {
    sort(std::less<value_type>());
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
constexpr auto erase(array<T, S, M>& c, auto&& k, char = {})
  noexcept(noexcept(
      erase_if(
        c,
        [](T const&) noexcept(noexcept(
            std::equal_to()(std::declval<T>(), std::declval<decltype(k)>())
          )
        )
        {
          return true;
        }
      )
    )
  )
  requires(requires{std::equal_to()(std::declval<T>(), k);})
{
  return erase_if(
    c,
    [&](auto&& v) noexcept(noexcept(std::equal_to()(v, k)))
    {
      return std::equal_to()(v, k);
    }
  );
}

template <typename T, std::size_t S, enum Method M>
constexpr auto erase(array<T, S, M>& c, T const& k)
  noexcept(noexcept(erase(c, k, {})))
{
  return erase(c, k, {});
}

template <typename T, std::size_t S, enum Method M>
constexpr auto erase_if(array<T, S, M>& c, auto pred)
  noexcept(
    noexcept(pred(std::declval<T>())) &&
    noexcept(c.erase(c.begin()))
  )
{
  typename array<T, S, M>::size_type r{};

  for (auto i(c.begin()); i.n() != c.l_;)
  {
    i = pred(*i) ? (++r, c.erase(i)) : std::next(i);
  }

  return r;
}

//////////////////////////////////////////////////////////////////////////////
template <typename T, std::size_t S, enum Method M>
constexpr void swap(array<T, S, M>& lhs, decltype(lhs) rhs) noexcept
  requires((NEW == M) || (USER == M))
{
  lhs.swap(rhs);
}

}

#endif // CA_ARRAY_HPP
