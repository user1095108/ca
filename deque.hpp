#ifndef CA_DEQUE_HPP
# define CA_DEQUE_HPP
# pragma once

#include <atomic>

#include <limits>

#include <algorithm> // inplace_merge()

#include "array.hpp"

namespace ca
{

template <typename T, std::size_t N, enum Method M = MEMBER>
class deque
{
  static_assert(N > 1);

public:
  using value_type = T;

  using difference_type = std::ptrdiff_t;
  using size_type = std::size_t;
  using reference = value_type&;
  using const_reference = value_type const&;

private:
  std::atomic<T*> first_;
  std::atomic<T*> last_;

  std::conditional_t<MEMBER == M, T[N], T*> a_;

  static constexpr auto next(auto const a, auto const p) noexcept
  {
    return p == &a[N - 1] ? a : p + 1;
  }

  static constexpr auto prev(auto const a, auto const p) noexcept
  {
    return p == a ? &a[N - 1] : p - 1;
  }

public:
  constexpr deque()
    noexcept(
      ((MEMBER == M) && std::is_nothrow_default_constructible_v<T[N]>) ||
      ((NEW == M) && noexcept(new T[N]))
    )
    requires((MEMBER == M) || (NEW == M))
  {
    if constexpr(NEW == M) a_ = new T[N];
    first_.store(a_, std::memory_order_relaxed);
    last_.store(a_, std::memory_order_relaxed);
  }

  constexpr explicit deque(T* const a) noexcept requires(USER == M):
    a_(a)
  {
    first_.store(a, std::memory_order_relaxed);
    last_.store(a, std::memory_order_relaxed);
  }

  constexpr ~deque()
    noexcept(
      ((MEMBER == M) && std::is_nothrow_destructible_v<T[N]>) ||
      ((NEW == M) && noexcept(new T[N])) ||
      (USER == M)
    )
  {
    if constexpr(NEW == M) delete [] a_;
  }

  //
  constexpr deque(deque const&) = delete;
  constexpr deque(deque&&) = delete;

  //
  constexpr deque& operator=(deque const& o) = delete;
  constexpr deque& operator=(deque&& o) = delete;

  //
  static constexpr size_type capacity() noexcept { return N - 1; }

  static constexpr size_type max_size() noexcept
  {
    return std::numeric_limits<difference_type>::max();
  }

  void clear() noexcept
  {
    first_.store(a_, std::memory_order::release);
    last_.store(a_, std::memory_order::release);
  }

  bool empty() const noexcept
  {
    return first_.load(std::memory_order::acquire) ==
      last_.load(std::memory_order::acquire);
  }

  bool full() const noexcept
  {
    return next(a_, last_.load(std::memory_order::acquire)) ==
      first_.load(std::memory_order::acquire);
  }

  size_type size() const noexcept
  {
    auto const n(
      last_.load(std::memory_order::acquire) -
      first_.load(std::memory_order::acquire)
    );

    return n < 0 ? N + n : n;
  }

  //
  bool pop_back(T& v)
    noexcept(std::is_nothrow_assignable_v<T&, T&&>)
  {
    for (auto l(last_.load(std::memory_order::acquire));;)
    {
      if (l == first_.load(std::memory_order::acquire))
      { // empty?
        return false;
      }
      else
      {
        if (auto const l1(prev(a_, l));
          last_.compare_exchange_weak(
            l,
            l1,
            std::memory_order::acq_rel,
            std::memory_order::acquire
          )
        )
        {
          v = std::move(*l1);

          return true;
        }
      }
    }
  }

  bool pop_front(T& v)
    noexcept(std::is_nothrow_assignable_v<T&, T&&>)
  {
    for (auto f(first_.load(std::memory_order::acquire));;)
    {
      if (f == last_.load(std::memory_order::acquire))
      { // empty?
        return false;
      }
      else
      {
        if (first_.compare_exchange_weak(
            f,
            next(a_, f),
            std::memory_order::acq_rel,
            std::memory_order::acquire
          )
        )
        {
          v = std::move(*f);

          return true;
        }
      }
    }
  }

  //
  constexpr void push_back(auto&& v)
    noexcept(std::is_nothrow_assignable_v<value_type&, decltype(v)>)
    requires(std::is_assignable_v<value_type&, decltype(v)>)
  {
    for (auto l(last_.load(std::memory_order::acquire));;)
    {
      if (last_.compare_exchange_weak(
          l,
          next(a_, l),
          std::memory_order::acq_rel,
          std::memory_order::acquire
        )
      )
      {
        *l = std::forward<decltype(v)>(v);

        for (auto f(first_.load(std::memory_order::acquire));
          f == last_.load(std::memory_order::acquire);)
        {
          if (first_.compare_exchange_weak(
              f,
              next(a_, f),
              std::memory_order::acq_rel,
              std::memory_order::acquire
            )
          )
          {
            break;
          }
        }

        break;
      }
    }
  }

  constexpr void push_front(auto&& v)
    noexcept(std::is_nothrow_assignable_v<value_type&, decltype(v)>)
    requires(std::is_assignable_v<value_type&, decltype(v)>)
  {
    for (auto f0(first_.load(std::memory_order::acquire));;)
    {
      auto f1(
        f0 == next(a_, last_.load(std::memory_order::acquire)) ?
          f0 :
          prev(a_, f0)
      );

      if (first_.compare_exchange_weak(
          f0,
          f1,
          std::memory_order::acq_rel,
          std::memory_order::acquire
        )
      )
      {
        *f1 = std::forward<decltype(v)>(v);

        break;
      }
    }
  }
};

}

#endif // CA_DEQUE_HPP
