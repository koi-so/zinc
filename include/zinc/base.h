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
template <typename TMove,
          typename MovedType = typename std::remove_reference<TMove>::type &&>
inline constexpr auto move(TMove &&value) -> MovedType {
  return static_cast<MovedType>(value);
};

template <typename T> inline constexpr auto drop(T &&) -> void {}

template <typename Type> inline constexpr void swap(Type &a, Type &b) {
  Type temp = drop(a);
  a = drop(b);
  b = drop(temp);
}

template <class TForward>
[[nodiscard]] constexpr auto
forward(std::remove_reference_t<TForward> &Arg) noexcept -> TForward && {
  return static_cast<TForward &&>(Arg);
}
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