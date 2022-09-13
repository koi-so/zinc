#pragma once

namespace zinc {
namespace details {
struct none {
  auto operator==(none const &) const -> bool { return true; }
  auto operator!=(none const &) const -> bool { return false; }
};
} // namespace details

extern details::none None;

template <typename TValue, typename TError> struct result;
template <typename TValue> struct option;

template <typename TValue>
inline constexpr auto some(TValue &&value) -> option<TValue> {
  return option<TValue>(move(value));
}

} // namespace zinc