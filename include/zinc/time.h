#pragma once

#include "base.h"
#include "checked_int.h"
#include "debug.h"

namespace zinc {
static const u32 NANOSECONDS_PER_SECOND = 1'000'000'000;
static const u32 NANOSECONDS_PER_MILLISECOND = 1'000'000;
static const u32 NANOSECONDS_PER_MICROSECOND = 1'000;
static const u64 MILLISECONDS_PER_SECOND = 1'000;
static const u64 MICROSECONDS_PER_SECOND = 1'000'000;

struct duration {
  u64 m_seconds{0};
  u32 m_nanoseconds{0};

  static duration SECOND;
  static duration MILLISECOND;
  static duration MICROSECOND;
  static duration NANOSECOND;
  static duration ZERO;
  static duration MAX;

  inline duration() = default;

  inline duration(u64 seconds, u32 nanoseconds) {
    auto s = ::zinc::checked_add(seconds, nanoseconds / NANOSECONDS_PER_SECOND);
    ZINC_ASSERTF(s.has_value(), "Overflow in duration()");
    m_seconds = s.value();
    m_nanoseconds = nanoseconds % NANOSECONDS_PER_SECOND;
  }

  inline static auto from_seconds(u64 m_seconds) -> duration {
    return {m_seconds, 0};
  }

  inline static auto from_milliseconds(u64 milliseconds) -> duration {
    return {milliseconds / MILLISECONDS_PER_SECOND,
            (u32)(milliseconds % MILLISECONDS_PER_SECOND) *
                NANOSECONDS_PER_MILLISECOND};
  }

  inline static auto from_microseconds(u64 microseconds) -> duration {
    return {microseconds / MICROSECONDS_PER_SECOND,
            (u32)(microseconds % MICROSECONDS_PER_SECOND) *
                NANOSECONDS_PER_MICROSECOND};
  }

  inline static auto from_nanoseconds(u64 m_nanoseconds) -> duration {
    return {m_nanoseconds / NANOSECONDS_PER_SECOND,
            (u32)m_nanoseconds % NANOSECONDS_PER_SECOND};
  }

  [[nodiscard]] inline auto is_zero() const -> bool {
    return m_seconds == 0 && m_nanoseconds == 0;
  }
  [[nodiscard]] inline auto as_seconds() const -> u64 { return m_seconds; }

  [[nodiscard]] inline auto subsecond_milliseconds() const -> u32 {
    return m_nanoseconds / NANOSECONDS_PER_MILLISECOND;
  }
  [[nodiscard]] inline auto subsecond_microseconds() const -> u32 {
    return m_nanoseconds / NANOSECONDS_PER_MICROSECOND;
  }
  [[nodiscard]] inline auto subsecond_nanoseconds() const -> u32 {
    return m_nanoseconds;
  }

  [[nodiscard]] inline auto as_milliseconds() const -> u64 {
    return m_seconds * MILLISECONDS_PER_SECOND + subsecond_milliseconds();
  }
  [[nodiscard]] inline auto as_microseconds() const -> u64 {
    return m_seconds * MICROSECONDS_PER_SECOND + subsecond_microseconds();
  }
  [[nodiscard]] inline auto as_nanoseconds() const -> u64 {
    return m_seconds * NANOSECONDS_PER_SECOND + subsecond_nanoseconds();
  }

  [[nodiscard]] inline auto checked_add(duration const &rhs) const
      -> option<duration> {
    auto added_seconds = ::zinc::checked_add(m_seconds, rhs.m_seconds);
    if (!added_seconds.has_value())
      return None;
    auto nanos = m_nanoseconds + rhs.m_nanoseconds;
    if (nanos >= NANOSECONDS_PER_SECOND) {
      nanos -= NANOSECONDS_PER_SECOND;
      added_seconds = ::zinc::checked_add(added_seconds.value(), 1);
      if (!added_seconds.has_value())
        return None;
    }
    ZINC_ASSERT(nanos < NANOSECONDS_PER_SECOND);
    return Some(duration(added_seconds.value(), nanos));
  }

  [[nodiscard]] inline auto saturating_add(duration const &rhs) const
      -> duration {
    if (auto result = checked_add(rhs))
      return result.value();
    return MAX;
  }

  [[nodiscard]] inline auto checked_sub(duration const &rhs) const
      -> option<duration> {
    if (auto subbed_seconds = ::zinc::checked_sub(m_seconds, rhs.m_seconds)) {
      u32 nano = 0;
      if (m_nanoseconds >= rhs.m_nanoseconds) {
        nano = m_nanoseconds - rhs.m_nanoseconds;
      } else {
        subbed_seconds = ::zinc::checked_sub(subbed_seconds.value(), 1);
        if (!subbed_seconds.has_value())
          return None;
        nano = NANOSECONDS_PER_SECOND + m_nanoseconds - rhs.m_nanoseconds;
      }
      ZINC_ASSERT(nano < NANOSECONDS_PER_SECOND);
      return Some(duration(subbed_seconds.value(), nano));
    }
    return None;
  }

  [[nodiscard]] inline auto saturating_sub(duration const &rhs) const
      -> duration {
    if (auto result = checked_sub(rhs))
      return result.value();
    return ZERO;
  }

  [[nodiscard]] inline auto checked_mul(u64 rhs) const -> option<duration> {
    auto total_nanoseconds = m_nanoseconds * rhs;
    auto extra_seconds = total_nanoseconds / NANOSECONDS_PER_SECOND;
    auto nanos = total_nanoseconds % NANOSECONDS_PER_SECOND;
    if (auto secs = ::zinc::checked_mul(m_seconds, rhs)) {
      secs = ::zinc::checked_add(secs.value(), extra_seconds);
      if (secs.has_value()) {
        ZINC_ASSERT(nanos < NANOSECONDS_PER_SECOND);
        return Some(duration(secs.value(), nanos));
      }
    }
    return None;
  }

  [[nodiscard]] inline auto saturating_mul(u64 rhs) const -> duration {
    if (auto result = checked_mul(rhs))
      return result.value();
    return MAX;
  }

  [[nodiscard]] inline auto checked_div(u64 rhs) const -> option<duration> {
    if (rhs == 0)
      return None;
    auto secs = m_seconds / rhs;
    auto carry = m_seconds - secs * rhs;
    auto extra_nanos = carry * NANOSECONDS_PER_SECOND / rhs;
    auto nanos = m_nanoseconds / rhs + extra_nanos;
    ZINC_ASSERT(nanos < NANOSECONDS_PER_SECOND);
    return Some(duration(secs, nanos));
  }

  template <typename TFloatType, typename = typename std::enable_if_t<
                                     std::is_floating_point_v<TFloatType>>>
  [[nodiscard]] inline auto as_seconds_floating() const -> TFloatType {
    return as<TFloatType>(m_seconds) +
           as<TFloatType>(m_nanoseconds) /
               as<TFloatType>(NANOSECONDS_PER_SECOND);
  }

  // operators
  [[nodiscard]] inline auto operator==(duration const &rhs) const -> bool {
    return m_seconds == rhs.m_seconds && m_nanoseconds == rhs.m_nanoseconds;
  }
  [[nodiscard]] inline auto operator!=(duration const &rhs) const -> bool {
    return !(*this == rhs);
  }
  [[nodiscard]] inline auto operator<(duration const &rhs) const -> bool {
    return m_seconds < rhs.m_seconds ||
           (m_seconds == rhs.m_seconds && m_nanoseconds < rhs.m_nanoseconds);
  }
  [[nodiscard]] inline auto operator<=(duration const &rhs) const -> bool {
    return *this < rhs || *this == rhs;
  }
  [[nodiscard]] inline auto operator>(duration const &rhs) const -> bool {
    return !(*this <= rhs);
  }
  [[nodiscard]] inline auto operator>=(duration const &rhs) const -> bool {
    return !(*this < rhs);
  }

  [[nodiscard]] inline auto operator+(duration const &rhs) const -> duration {
    return saturating_add(rhs);
  }
  [[nodiscard]] inline auto operator-(duration const &rhs) const -> duration {
    return saturating_sub(rhs);
  }
  [[nodiscard]] inline auto operator*(u64 rhs) const -> duration {
    return saturating_mul(rhs);
  }
  [[nodiscard]] inline auto operator/(u64 rhs) const -> duration {
    return checked_div(rhs).value();
  }
};
} // namespace zinc