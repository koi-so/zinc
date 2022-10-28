#pragma once

#include "zinc.h"

namespace zinc {
namespace detail {
template <class T> constexpr auto FUN(T &t) noexcept -> T & { return t; }
template <class T> void FUN(T &&) = delete;
} // namespace detail

template <class T> class ref_wrapper {
public:
  // types
  using type = T;

  // construct/copy/destroy
  template <class U,
            class = decltype(detail::FUN<T>(std::declval<U>()),
                             std::enable_if_t<!std::is_same_v<
                                 ref_wrapper,
                                 std::remove_cv<std::remove_reference<U>>>>())>
  constexpr ref_wrapper(U &&u) noexcept(
      noexcept(detail::FUN<T>(std::forward<U>(u))))
      : m_ptr(std::addressof(detail::FUN<T>(std::forward<U>(u)))) {}
  ref_wrapper(const ref_wrapper &) noexcept = default;

  // assignment
  auto operator=(const ref_wrapper &x) noexcept -> ref_wrapper & = default;

  // access
  constexpr operator T &() const noexcept { return *m_ptr; }
  constexpr auto get() const noexcept -> T & { return *m_ptr; }

  template <class... ArgTypes>
  constexpr auto operator()(ArgTypes &&...args) const
      -> std::invoke_result_t<T &, ArgTypes...> {
    return std::invoke(get(), std::forward<ArgTypes>(args)...);
  }

private:
  T *m_ptr;
};

// deduction guides
template <class T> ref_wrapper(T &) -> ref_wrapper<T>;
} // namespace zinc