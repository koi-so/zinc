#pragma once

#include "base.h"
#include "func.h"
#include "option_result_fwd.h"
#include <iostream>

namespace zinc {
namespace details {
template <typename TPayload> struct success {
  success(const TPayload &payload) : m_payload(payload) {}
  success(TPayload &&payload) : m_payload(payload) {}

private:
  template <typename TValue, typename TError> friend struct result;
  TPayload m_payload;
};

template <typename TPayload> struct error {
  error(const TPayload &payload) : m_payload(payload) {}
  error(TPayload &&payload) : m_payload(payload) {}

private:
  template <typename TValue, typename TError> friend struct result;
  TPayload m_payload;
};
} // namespace details

template <typename TValue> auto Ok(TValue &&value) {
  return details::success<TValue>(move(value));
}

template <typename TValue> auto Ok(const TValue &value) {
  return details::success<TValue>(value);
}

template <typename TError> auto Err(TError &&value) {
  return details::error<TError>(move(value));
}

template <typename TError> auto Err(const TError &value) {
  return details::error<TError>(value);
}

template <typename TValue, typename TError> struct result {
public:
  result() = default;
  result(details::success<TValue> okr) : tag(Tag::Ok), m_value(okr.m_payload) {}
  result(details::error<TValue> okr) : tag(Tag::Err), m_error(okr.m_payload) {}
  [[nodiscard]] auto is_ok() const -> bool { return tag == Tag::Ok; }
  auto is_ok_and(func<bool(TValue const &)> fn) -> bool {
    if (is_ok() && fn(m_value)) {
      return true;
    }
    return false;
  }

  [[nodiscard]] constexpr auto is_err() const -> bool {
    return tag == Tag::Err;
  }
  auto is_err_and(func<bool(TError const &)> fn) -> bool {
    if (is_err() && fn(m_error)) {
      return true;
    }
    return false;
  }

  [[nodiscard]] constexpr auto ok() const -> option<TValue> {
    if (is_ok()) {
      return move(some(m_value));
    }
    return move(None);
  }
  [[nodiscard]] constexpr auto err() const -> option<TError> {
    if (is_err()) {
      return move(some(m_error));
    }
    return move(None);
  }

  template <typename TOtherValue>
  [[nodiscard]] constexpr auto map(func<TOtherValue(TValue)> fn) const
      -> result<TOtherValue, TError> {
    if (is_ok()) {
      return move(Ok(fn(m_value)));
    }
    return move(Err(m_error));
  }

  template <typename TOtherValue>
  [[nodiscard]] constexpr auto map_or(func<TOtherValue(TValue)> fn,
                                      TOtherValue &&fallback) const
      -> TOtherValue {
    if (is_ok()) {
      return move(fn(m_value));
    }
    return move(fallback);
  }

  template <typename TOtherValue>
  [[nodiscard]] constexpr auto map_or_else(func<TOtherValue(TError)> fallback,
                                           func<TOtherValue(TValue)> fn) const
      -> TOtherValue {
    if (is_ok()) {
      return move(fn(m_value));
    }
    return move(fallback(m_error));
  }

  template <typename TOtherError>
  [[nodiscard]] constexpr auto map_err(func<TOtherError(TError)> fn) const
      -> result<TValue, TOtherError> {
    if (is_err()) {
      return move(zinc::Err(fn(m_error)));
    }
    return move(zinc::Ok(m_value));
  }

  [[nodiscard]] constexpr auto inspect(func<void(TValue const &)> fn) const
      -> result {
    if (is_ok()) {
      fn(m_value);
    }
    return *this;
  }

  [[nodiscard]] constexpr auto inspect_err(func<void(TError const &)> fn) const
      -> result {
    if (is_err()) {
      fn(m_error);
    }
    return *this;
  }

private:
  enum Tag { Ok, Err } tag = Tag::Err;
  union {
    TValue m_value;
    TError m_error;
  };
};
} // namespace zinc