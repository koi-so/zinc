#pragma once

#include "base.h"

namespace zinc {
template <typename TValue> struct unique {
public:
  template <typename OtherDataType> friend struct unique;

  explicit unique(TValue *raw_ptr) noexcept : m_raw_ptr(std::move(raw_ptr)) {}

  unique(std::nullptr_t) noexcept : m_raw_ptr(nullptr) {}
  unique() noexcept : m_raw_ptr(nullptr) {}

  ~unique() {
    if (m_raw_ptr)
      delete (m_raw_ptr);
  }

  unique(const unique &other) = delete;
  auto operator=(const unique &other) -> unique & = delete;

  void swap(unique &other_owned) noexcept {
    std::swap(m_raw_ptr, other_owned.m_raw_ptr);
  }

  unique(unique &&other) noexcept : m_raw_ptr(other.m_raw_ptr) {
    other.m_raw_ptr = nullptr;
  }

  template <typename OtherDataType,
            typename = safe_upcast<OtherDataType, TValue>>
  unique(unique<OtherDataType> &&other) noexcept : m_raw_ptr(other.release()) {}

  auto operator=(unique &&other) noexcept -> unique & {
    other.swap(*this);
    return *this;
  }

  template <typename OtherDataType>
  auto operator=(unique<OtherDataType> &&other) noexcept -> unique & {
    other.swap(*this);
    return *this;
  }

  [[nodiscard]] explicit operator bool() const noexcept {
    return m_raw_ptr != nullptr;
  }

  [[nodiscard]] auto release() noexcept -> TValue * {
    return std::exchange(m_raw_ptr, nullptr);
  }

  [[nodiscard]] auto get() const noexcept -> TValue * { return m_raw_ptr; }

  void reset(TValue *other_owned) {
    m_deleter(m_raw_ptr);
    m_raw_ptr = nullptr;
    std::swap(m_raw_ptr, other_owned);
  }

  // operators
  [[nodiscard]] auto operator->() const noexcept -> TValue * {
    return m_raw_ptr;
  }
  [[nodiscard]] auto operator*() const noexcept -> TValue & {
    return *m_raw_ptr;
  }

private:
  TValue *m_raw_ptr{nullptr};
};

template <typename TValue, typename... Args>
auto MakeUnique(Args &&...args) -> unique<TValue> {
  return unique<TValue>(new TValue(std::forward<Args>(args)...));
}
} // namespace zinc