#pragma once

#include "zinc/base.h"

namespace zinc {
struct ref_count {
  ref_count() = default;
  ref_count(ref_count const &count) { m_count = count.m_count; }

  void swap(ref_count &other) noexcept { std::swap(m_count, other.m_count); }

  [[nodiscard]] auto use_count() const noexcept -> usize { return m_count; }

  auto acquire() noexcept -> void { ++m_count; }
  auto release() noexcept -> bool {
    --m_count;
    return m_count != 0;
  }

  operator bool() const noexcept { return m_count != 0; }

private:
  usize m_count{0};
};

template <typename T> struct ref {
public:
  ref() = default;
  ref(T *handle) : m_handle(handle) {
    m_count = new ref_count();
    acquire();
  }
  ref(T *handle, ref_count *count) : m_count(count), m_handle(handle) {
    acquire();
  }
  ~ref() { release(); }

  ref(ref const &other) : m_count(other.m_count), m_handle(other.m_handle) {
    acquire();
  }
  ref(ref &&other) : m_count(other.m_count), m_handle(other.m_handle) {
    other.m_handle = nullptr;
  }

  auto operator=(ref const &other) -> ref<T> & {
    if (&other != this) {
      release();
      m_handle = other.get();
      m_count = other.m_count;
      acquire();
    }

    return as<T &>(*this);
  }

  auto operator=(ref &&other) -> ref<T> & {
    if (&other != this) {
      if (m_handle) {
        release();
      }
      m_count = other.m_count;
      m_handle = other.m_handle;
      other.m_handle = nullptr;
    }

    return as<T &>(*this);
  }

  ref(std::nullptr_t) { m_handle = nullptr; }
  auto operator=(std::nullptr_t) -> ref<T> & {
    release();
    m_handle = nullptr;
    return as<T &>(*this);
  }

  operator bool() const { return m_handle != nullptr; }

  auto operator==(std::nullptr_t) const -> bool { return m_handle == nullptr; }
  auto operator!=(std::nullptr_t) const -> bool { return m_handle != nullptr; }

  [[nodiscard]] auto use_count() const noexcept -> usize {
    return m_count->use_count();
  }
  [[nodiscard]] auto get() const -> T * { return m_handle; }

  [[nodiscard]] auto operator->() const -> T * { return m_handle; }
  [[nodiscard]] auto operator*() const -> T * { return *m_handle; }

  void acquire() {
    if (m_handle) {
      m_count->acquire();
    }
  }
  void release() {
    if (m_handle) {
      if (!m_count->release()) {
        delete m_handle;
        delete m_count;
      }
    }
  }

private:
  T *m_handle{nullptr};
  ref_count *m_count;
};

template <typename T, typename... Args>
auto make_ref(Args &&...args) -> ref<T> {
  return std::move(ref<T>(new T(std::forward<Args>(args)...)));
}
} // namespace zinc