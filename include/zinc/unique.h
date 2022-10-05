#pragma once

#include "base.h"

namespace zinc {
template <typename T> struct unique {
public:
  template <typename OtherDataType> friend struct unique;

  explicit unique(T *raw_ptr) noexcept : m_raw_ptr(std::move(raw_ptr)) {}

  unique(std::nullptr_t) noexcept : m_raw_ptr(nullptr) {}
  unique() noexcept : m_raw_ptr(nullptr) {}

  ~unique() { cleanup(); }

  unique(const unique &other) = delete;
  auto operator=(const unique &other) -> unique & = delete;

  void swap(unique &other_owned) noexcept {
    std::swap(m_raw_ptr, other_owned.m_raw_ptr);
  }

  unique(unique &&other) noexcept { std::swap(m_raw_ptr, other.m_raw_ptr); }

  template <typename OtherDataType, typename = safe_upcast<OtherDataType, T>>
  unique(unique<OtherDataType> &&other) noexcept : m_raw_ptr(other.release()) {}

  auto operator=(unique &&other) noexcept -> unique & {
    cleanup();
    other.swap(*this);
    return *this;
  }

  template <typename OtherDataType>
  auto operator=(unique<OtherDataType> &&other) noexcept -> unique & {
    cleanup();
    other.swap(*this);
    return *this;
  }

  [[nodiscard]] explicit operator bool() const noexcept {
    return m_raw_ptr != nullptr;
  }

  [[nodiscard]] auto release() noexcept -> T * {
    return std::exchange(m_raw_ptr, nullptr);
  }

  [[nodiscard]] auto get() const noexcept -> T * { return m_raw_ptr; }

  void reset(T *other_owned) {
    m_raw_ptr = nullptr;
    std::swap(m_raw_ptr, other_owned);
  }

  // operators
  [[nodiscard]] auto operator->() const noexcept -> T * { return m_raw_ptr; }
  [[nodiscard]] auto operator*() const noexcept -> T & { return *m_raw_ptr; }

private:
  void cleanup() {
    if (m_raw_ptr != nullptr)
      delete (m_raw_ptr);
  }
  T *m_raw_ptr{nullptr};
};

template <typename T, typename... Args>
auto make_unique(Args &&...args) -> unique<T> {
  return std::move(unique<T>(new T(std::forward<Args>(args)...)));
}
} // namespace zinc