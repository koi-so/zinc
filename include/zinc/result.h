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
  inline result() = default;
  inline result(details::success<TValue> okr)
      : tag(Tag::Ok), m_value(okr.m_payload) {}
  inline result(details::error<TValue> okr)
      : tag(Tag::Err), m_error(okr.m_payload) {}
  [[nodiscard]] inline auto is_ok() const -> bool { return tag == Tag::Ok; }

  inline auto is_ok_and(func<bool(TValue const &)> fn) -> bool {
    if (is_ok() && fn(m_value)) {
      return true;
    }
    return false;
  }

  [[nodiscard]] inline auto is_err() const -> bool { return tag == Tag::Err; }
  inline auto is_err_and(func<bool(TError const &)> fn) -> bool {
    if (is_err() && fn(m_error)) {
      return true;
    }
    return false;
  }

  [[nodiscard]] inline auto ok() const -> option<TValue> {
    if (is_ok()) {
      return move(some(m_value));
    }
    return move(None);
  }
  [[nodiscard]] inline auto err() const -> option<TError> {
    if (is_err()) {
      return move(some(m_error));
    }
    return move(None);
  }

  template <typename TOtherValue>
  [[nodiscard]] inline auto map(func<TOtherValue(TValue)> fn) const
      -> result<TOtherValue, TError> {
    if (is_ok()) {
      return move(Ok(fn(m_value)));
    }
    return move(Err(m_error));
  }

  template <typename TOtherValue>
  [[nodiscard]] inline auto map_or(func<TOtherValue(TValue)> fn,
                                   TOtherValue &&fallback) const
      -> TOtherValue {
    if (is_ok()) {
      return move(fn(m_value));
    }
    return move(fallback);
  }

  template <typename TOtherValue>
  [[nodiscard]] inline auto map_or_else(func<TOtherValue(TError)> fallback,
                                        func<TOtherValue(TValue)> fn) const
      -> TOtherValue {
    if (is_ok()) {
      return move(fn(m_value));
    }
    return move(fallback(m_error));
  }

  template <typename TOtherError>
  [[nodiscard]] inline auto map_err(func<TOtherError(TError)> fn) const
      -> result<TValue, TOtherError> {
    if (is_err()) {
      return move(zinc::Err(fn(m_error)));
    }
    return move(zinc::Ok(m_value));
  }

  [[nodiscard]] inline auto inspect(func<void(TValue const &)> fn) const
      -> result {
    if (is_ok()) {
      fn(m_value);
    }
    return *this;
  }

  [[nodiscard]] inline auto inspect_err(func<void(TError const &)> fn) const
      -> result {
    if (is_err()) {
      fn(m_error);
    }
    return *this;
  }

  [[nodiscard]] inline auto expect(const char *msg) const -> TValue {
    if (is_ok()) {
      return move(m_value);
    }
    std::cerr << msg << std::endl;
    std::terminate();
  }

  [[nodiscard]] inline auto unwrap() const -> TValue {
    if (is_ok()) {
      return move(m_value);
    }
    std::cerr << "called `Result::unwrap()` on an `Err` value: " << m_error
              << std::endl;
    std::terminate();
  }

  [[nodiscard]] inline auto unwrap_or_default() const -> TValue {
    if (is_ok()) {
      return move(m_value);
    }
    return {};
  }

  [[nodiscard]] inline auto expect_err(const char *msg) const -> TError {
    if (is_err()) {
      return move(m_error);
    }
    std::cerr << msg << std::endl;
    std::terminate();
  }

  [[nodiscard]] inline auto unwrap_err() const -> TError {
    if (is_err()) {
      return move(m_error);
    }
    std::cerr << "called `Result::unwrap_err()` on an `Ok` value: " << m_value
              << std::endl;
    std::terminate();
  }

  template <typename TOtherValue>
  [[nodiscard]] inline auto _and(result<TOtherValue, TError> &&r) const
      -> result<TOtherValue, TError> {
    if (is_ok()) {
      return move(r);
    }
    return move(Err(m_error));
  }

  template <typename TOtherValue>
  [[nodiscard]] inline auto
  and_then(func<result<TOtherValue, TError>(TValue const &)> fn) const
      -> result<TOtherValue, TError> {
    if (is_ok()) {
      return move(fn(m_value));
    }
    return move(Err(m_error));
  }

  template <typename TOtherError>
  [[nodiscard]] inline auto _or(result<TValue, TOtherError> &&r) const
      -> result<TValue, TOtherError> {
    if (is_ok()) {
      return move(Ok(m_value));
    }
    return move(r);
  }

  template <typename TOtherError>
  [[nodiscard]] inline auto
  or_else(func<result<TValue, TOtherError>(TError const &)> fn) const
      -> result<TValue, TOtherError> {
    if (is_ok()) {
      return move(Ok(m_value));
    }
    return move(fn(m_error));
  }

  [[nodiscard]] inline auto unwrap_or(TValue &&fallback) const -> TValue {
    if (is_ok()) {
      return move(m_value);
    }
    return move(fallback);
  }

  [[nodiscard]] inline auto
  unwrap_or_else(func<TValue(TError const &)> fn) const -> TValue {
    if (is_ok()) {
      return move(m_value);
    }
    return move(fn(m_error));
  }

  [[nodiscard]] inline auto unwrap_unchecked() const -> TValue {
    // TODO: add debug macros
    if (is_ok()) {
      return move(m_value);
    }
  }

private:
  enum Tag { Ok, Err } tag = Tag::Err;
  union {
    TValue m_value;
    TError m_error;
  };
};
} // namespace zinc