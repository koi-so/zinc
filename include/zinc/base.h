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

namespace zinc {

struct panic_data {
  const char *msg;
};

template <typename T> inline constexpr auto drop(T &&) -> void {}

template <typename TAs>
[[nodiscard]] inline constexpr auto as(TAs &&value) noexcept -> TAs && {
  return static_cast<TAs &&>(value);
}

inline auto panic(char const *msg) -> void { throw panic_data{msg}; }

template <typename From, typename To>
using safe_upcast = std::conjunction<std::is_convertible<To *, From *>,
                                     std::negation<std::is_array<To *>>>;

} // namespace zinc

// nice typedefs

// Unsigned int types.
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

// Signed int types.
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef u8 byte;

// Floating point types
typedef float f32;
typedef double f64;

typedef size_t usize;

typedef uintptr_t uptr;
typedef intptr_t iptr;
typedef void *vptr;
typedef ptrdiff_t ptrdiff;

#include "platform.h"

// Shared library
#if ZINC_CONFIG_SHARED_LIB
#if ZINC_COMPILER_MSVC
#ifdef ZINC_EXPORTS
#define ZINC_API __declspec(dllexport)
#else
#define ZINC_API __declspec(dllimport)
#endif
#else
#define ZINC_API __attribute__((visibility("default")))
#endif
#else
#define ZINC_API
#endif