#pragma once

#include "base.h"
#include "option_result_fwd.h"

namespace zinc {

template <typename TValue> struct option {
  option() = default;
  option(const details::none &none) {}
  option(details::none &none) {}
  option(TValue value) : m_value(value), m_has_value(true) {}

  option(option const &other) = default;
  option(option &&other) = default;

  template <typename TOtherValue,
            typename = std::is_convertible<TOtherValue, TValue>>
  option(option<TOtherValue> const &other)
      : m_value(other.m_value), m_has_value(other.m_has_value) {}

  template <typename TOtherValue,
            typename = std::is_convertible<TOtherValue, TValue>>
  option(option<TOtherValue> &&other)
      : m_value(other.m_value), m_has_value(other.m_has_value) {}

  auto operator=(option const &other) -> option & = default;
  auto operator=(option &&other) -> option & = default;

  [[nodiscard]] constexpr auto has_value() const -> bool { return m_has_value; }
  [[nodiscard]] constexpr auto value() const -> TValue const & {
    return m_value;
  }

  operator bool() const { return m_has_value; }

private:
  template <typename TOtherValue> friend struct option;

  bool m_has_value{false};
  TValue m_value;
};
} // namespace zinc