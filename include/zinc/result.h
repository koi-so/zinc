#pragma once

#include "base.h"
#include "func.h"

namespace zinc {
template <typename TValue, typename TError> struct result;
namespace details {
template <typename TPayload> struct ok_result {
  ok_result(const TPayload &payload) : m_payload(payload) {}
  ok_result(TPayload &&payload) : m_payload(payload) {}

private:
  template <typename TValue, typename TError> friend struct result;
  TPayload m_payload;
};
} // namespace details

template <typename TValue, typename TError> struct result {
public:
  result() = default;
  result(details::ok_result<TValue> okr) : tag(Tag::Ok) {
    m_value = okr.m_payload;
  }
  [[nodiscard]] auto is_ok() const -> bool { return tag == Tag::Ok; }
  auto is_ok_and(func<bool(TValue const &)> fn) -> bool {
    if (is_ok() && fn(m_value)) {
      return true;
    }
    return false;
  }

  [[nodiscard]] constexpr auto is_err() const -> bool {
    return !tag == Tag::Err;
  }
  auto is_err_and(func<bool(TError const &)> fn) -> bool {
    if (is_err() && fn(m_error)) {
      return true;
    }
    return false;
  }

  [[nodiscard]] constexpr auto ok() const -> TValue const & { return m_value; }
  [[nodiscard]] constexpr auto err() const -> TError const & { return m_error; }

private:
  enum Tag { Ok, Err } tag = Tag::Err;
  union {
    TValue m_value;
    TError m_error;
  };
};

template <typename TValue> auto ok(TValue &&value) {
  return details::ok_result<TValue>(move(value));
}

template <typename TValue> auto ok(const TValue &value) {
  return details::ok_result<TValue>(value);
}
} // namespace zinc