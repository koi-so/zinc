#pragma once

#include "base.h"

namespace zinc {
static const u32 NANOSECONDS_PER_SECOND = 1'000'000'000;
static const u32 NANOSECONDS_PER_MILLISECOND = 1'000'000;
static const u32 NANOSECONDS_PER_MICROSECOND = 1'000;
static const u64 MILLISECONDS_PER_SECOND = 1'000;
static const u64 MICROSECONDS_PER_SECOND = 1'000'000;

struct duration {
  u64 seconds;
  u32 nanoseconds;

  static duration SECOND;
  static duration MILLISECOND;
  static duration MICROSECOND;
  static duration NANOSECOND;
  static duration ZERO;
  static duration MAX;

  inline duration(u64 seconds, u32 nanoseconds)
      : seconds(seconds + nanoseconds / NANOSECONDS_PER_SECOND),
        nanoseconds(nanoseconds % NANOSECONDS_PER_SECOND) {}

  inline static duration from_seconds(u64 seconds) {
    return duration(seconds, 0);
  }

  inline static duration from_milliseconds(u64 milliseconds) {
    return duration(milliseconds / MILLISECONDS_PER_SECOND,
                    (milliseconds % MILLISECONDS_PER_SECOND) *
                        NANOSECONDS_PER_MILLISECOND);
  }

  inline static duration from_microseconds(u64 microseconds) {
    return duration(microseconds / MICROSECONDS_PER_SECOND,
                    (microseconds % MICROSECONDS_PER_SECOND) *
                        NANOSECONDS_PER_MICROSECOND);
  }

  inline static duration from_nanoseconds(u64 nanoseconds) {
    return duration(nanoseconds / NANOSECONDS_PER_SECOND,
                    nanoseconds % NANOSECONDS_PER_SECOND);
  }

  inline auto is_zero() const -> bool {
    return seconds == 0 && nanoseconds == 0;
  }
  inline auto as_seconds() const -> u64 { return seconds; }

  inline auto subsecond_milliseconds() const -> u32 {
    return nanoseconds / NANOSECONDS_PER_MILLISECOND;
  }
  inline auto subsecond_microseconds() const -> u32 {
    return nanoseconds / NANOSECONDS_PER_MICROSECOND;
  }
  inline auto subsecond_nanoseconds() const -> u32 { return nanoseconds; }

  inline auto as_milliseconds() const -> u64 {
    return seconds * MILLISECONDS_PER_SECOND + subsecond_milliseconds();
  }
  inline auto as_microseconds() const -> u64 {
    return seconds * MICROSECONDS_PER_SECOND + subsecond_microseconds();
  }
  inline auto as_nanoseconds() const -> u64 {
    return seconds * NANOSECONDS_PER_SECOND + subsecond_nanoseconds();
  }
};
} // namespace zinc