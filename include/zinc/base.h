#pragma once

// mostly headers found on freestanding implementations of C++
#include <atomic>
#include <cfloat>
#include <climits>
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <initializer_list>
#include <new>
#include <type_traits>
#include <utility>

// These are only available on devices that have a functional kernel
#include <cstdio>
#include <cstring>
#include <memory>

namespace zinc {

struct panic_data {
  const char *msg;
};

template <typename TValue> inline constexpr auto drop(TValue &&) -> void {}

template <typename TAs>
[[nodiscard]] inline constexpr auto as(TAs &&value) noexcept -> TAs && {
  return static_cast<TAs &&>(value);
}

inline auto panic(char const *msg) -> void { throw panic_data{msg}; }

template <typename From, typename To>
using safe_upcast = std::conjunction<std::is_convertible<To *, From *>,
                                     std::negation<std::is_array<To *>>>;

struct non_copyable {
  non_copyable() = default;
  non_copyable(const non_copyable &) = delete;
  auto operator=(const non_copyable &) -> non_copyable & = delete;
};

// checked_delete
template <class TValue> inline void checked_delete(TValue *x) noexcept {
  // intentionally complex - simplification causes regressions
  using type_must_be_complete = char[sizeof(TValue) ? 1 : -1];
  (void)sizeof(type_must_be_complete);
  delete x;
}

template <class TValue> inline void checked_array_delete(TValue *x) noexcept {
  using type_must_be_complete = char[sizeof(TValue) ? 1 : -1];
  (void)sizeof(type_must_be_complete);
  delete[] x;
}

template <class TValue> struct checked_deleter {
  using result_type = void;
  using argument_type = TValue *;

  void operator()(TValue *x) const noexcept { zinc::checked_delete(x); }
};

template <class TValue> struct checked_array_deleter {
  using result_type = void;
  using argument_type = TValue *;

  void operator()(TValue *x) const noexcept { zinc::checked_array_delete(x); }
};
} // namespace zinc

// Unsigned int types.
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

// Signed int types.
using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using byte = u8;

// Floating point types
using f32 = float;
using f64 = double;

#include "platform.h"

#if ZINC_ARCH_64BIT
using usize = u64;
using isize = i64;
#else
using usize = u32;
using isize = i32;
#endif

// pointer types
using uptr = uintptr_t;
using iptr = intptr_t;
using vptr = void *;
using ptrdiff = ptrdiff_t;
