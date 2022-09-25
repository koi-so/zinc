#pragma once

#include "base.h"
#include "debug.h"
#include "func.h"

namespace zinc {
struct nullopt_t {
  struct init {};
  constexpr explicit nullopt_t(init) {}
};

extern nullopt_t None;

template <typename TValue> struct option {
public:
  template <typename TOtherValue> friend struct option;

  option() noexcept = default;
  option(nullopt_t) noexcept {};

  option(TValue &&value) : m_value(std::move(value)), m_has_value(true) {}
  option(TValue const &value) : m_value(value), m_has_value(true) {}

  template <typename TOtherValue, typename = safe_upcast<TOtherValue, TValue>>
  option(option<TOtherValue> &&other) {
    if (other.has_value())
      m_value = std::move(other.m_value);
    m_has_value = other.m_has_value;
  }
  template <typename TOtherValue, typename = safe_upcast<TOtherValue, TValue>>
  option(option<TOtherValue> const &other) {
    if (other.has_value())
      m_value = other.m_value;
    m_has_value = other.m_has_value;
  }

  [[nodiscard]] auto has_value() const noexcept -> bool { return m_has_value; }

  [[nodiscard]] auto value() const & -> TValue const & {
    ZINC_ASSERT(has_value());
    return m_value;
  }
  [[nodiscard]] auto value() & -> TValue & {
    ZINC_ASSERT(has_value());
    return m_value;
  }
  [[nodiscard]] auto value() && -> TValue && {
    ZINC_ASSERT(has_value());
    return std::move(m_value);
  }

  [[nodiscard]] auto
  value_or(TValue const &default_value) const & -> TValue const & {
    if (has_value())
      return m_value;
    return default_value;
  }
  [[nodiscard]] auto value_or(TValue const &default_value) && -> TValue {
    if (has_value())
      return std::move(m_value);
    return default_value;
  }
  [[nodiscard]] auto
  value_or(TValue &&default_value) const & -> TValue const & {
    if (has_value())
      return m_value;
    return default_value;
  }
  [[nodiscard]] auto value_or(TValue &&default_value) && -> TValue {
    if (has_value())
      return std::move(m_value);
    return default_value;
  }

  template <typename TMapValue>
  [[nodiscard]] auto
  map(func<TMapValue(TValue const &)> &&func) const & -> option<TMapValue> {
    if (has_value())
      return option<TMapValue>(func(value()));
    return None;
  }

  operator bool() const noexcept { return has_value(); }

private:
  bool m_has_value{false};
  TValue m_value;
};

template <typename TValue>
[[nodiscard]] inline auto Some(TValue &&value) -> option<TValue> {
  return option<TValue>(std::forward<TValue>(value));
}
} // namespace zinc