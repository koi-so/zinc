#pragma once

#include "base.h"

#include "option.h"

#define SAFEINT_ASSERT (void)
#define SAFE_INT_HAS_EXCEPTIONS 0
#include "third_party/safe_int.h"

namespace zinc {
template <typename T, typename U>
[[nodiscard]] inline auto checked_add(T a, U b) -> option<T> {
  T result;
  if (SafeAdd(a, b, result))
    return result;
  return None;
}

template <typename T, typename U>
[[nodiscard]] inline auto checked_sub(T a, U b) -> option<T> {
  T result;
  if (SafeSubtract(a, b, result))
    return result;
  return None;
}

template <typename T, typename U>
[[nodiscard]] inline auto checked_mul(T a, U b) -> option<T> {
  T result;
  if (SafeMultiply(a, b, result))
    return result;
  return None;
}

template <typename T, typename U>
[[nodiscard]] inline auto checked_div(T a, U b) -> option<T> {
  T result;
  if (SafeDivide(a, b, result))
    return result;
  return None;
}
} // namespace zinc