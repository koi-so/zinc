#pragma once

#include "base.h"

#include <algorithm>

namespace zinc {
// TODO: Actually reimplement this
template <typename TValue, typename TCompare>
inline auto sort(TValue *begin, TValue *end, TCompare compare) -> void {
  std::sort(begin, end, compare);
}

template <typename TValue, typename TCompare>
inline auto sort(TValue *begin, usize count, TCompare compare) -> void {
  std::sort(begin, begin + count, compare);
}

template <typename TValue, typename TCompare>
inline auto sort(TValue *begin, TValue *end) -> void {
  std::sort(begin, end);
}

template <typename TValue, typename TCompare>
inline auto sort(TValue *begin, usize count) -> void {
  std::sort(begin, begin + count);
}
} // namespace zinc