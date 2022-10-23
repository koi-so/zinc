#pragma once

#include "zinc/base.h"

namespace zinc {
struct shared_count {
  shared_count() = default;
  shared_count(shared_count const &count) {
    m_count.store(count.m_count.load());
  }

  [[nodiscard]] auto use_count() const noexcept -> usize { return m_count; }

  auto acquire() noexcept -> void { ++m_count; }
  auto release() noexcept -> bool {
    --m_count;
    return m_count != 0;
  }

  operator bool() const noexcept { return m_count != 0; }

private:
  std::atomic<usize> m_count{0};
};

template <typename T, typename D = void (*)(T *)> struct shared {
public:
  shared() = default;
  template <typename U>
  shared(U *handle, D deleter = nullptr)
      : m_handle(handle), m_deleter(deleter) {
    m_count = new shared_count();
    acquire();
  }
  template <typename U>
  shared(U *handle, shared_count *count, D deleter = nullptr)
      : m_count(count), m_handle(handle), m_deleter(deleter) {
    acquire();
  }
  ~shared() { release(); }

  shared(shared const &other)
      : m_count(other.m_count), m_handle(other.m_handle) {
    acquire();
  }
  shared(shared &&other) : m_count(other.m_count), m_handle(other.m_handle) {
    other.m_handle = nullptr;
  }

  auto operator=(shared const &other) -> shared<T> & {
    if (&other != this) {
      release();
      m_handle = other.get();
      m_count = other.m_count;
      acquire();
    }

    return as<T &>(*this);
  }

  auto operator=(shared &&other) -> shared<T> & {
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

  shared(std::nullptr_t) { m_handle = nullptr; }
  auto operator=(std::nullptr_t) -> shared<T> & {
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
  [[nodiscard]] auto get() const -> T & { return *m_handle; }

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
        if (m_deleter)
          m_deleter(m_handle);
        else
          delete[] m_handle;
        delete m_count;
      }
    }
  }

private:
  T *m_handle{nullptr};
  shared_count *m_count{nullptr};
  D m_deleter{nullptr};
};

template <typename T, typename... Args>
auto make_shared(Args &&...args) -> shared<T> {
  return std::move(shared<T>(new T(std::forward<Args>(args)...)));
}
} // namespace zinc