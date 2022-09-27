#pragma once

#include "allocator/prelude.h"
#include "base.h"
#include "debug.h"
#include "vec.h"

namespace zinc {
template <typename TAllocator = sys_allocator<char>> struct string {
public:
  using size_type = usize;

  template <
      typename TProxy = TAllocator,
      typename = std::enable_if_t<std::is_nothrow_constructible_v<TProxy>>>
  inline string() : m_buffer() {}

  inline explicit string(TAllocator &allocator) : m_buffer(allocator) {}

  inline explicit string(size_type const count,
                         TAllocator &allocator = TAllocator())
      : m_buffer(count + 1, allocator) {
    m_buffer[count] = '\0';
  }

  inline string(string const &other) : m_buffer(other.m_buffer) {}
  inline string(string &&other) : m_buffer(std::move(other.m_buffer)) {}

  template <
      typename TProxy = TAllocator,
      typename = std::enable_if_t<std::is_nothrow_constructible_v<TProxy>>>
  inline string(char const *str) : string(str, TAllocator()) {}
  inline string(char const *str, TAllocator &allocator)
      : string(str, strlen(str), allocator) {}

  template <
      typename TProxy = TAllocator,
      typename = std::enable_if_t<std::is_nothrow_constructible_v<TProxy>>>
  inline string(char const *str, size_type const len) : string(str, len, {}) {}
  inline string(char const *str, size_type const len, TAllocator &allocator)
      : m_buffer(allocator) {
    m_buffer.resize(len + 1);
    memcpy(m_buffer.data(), str, len);
    m_buffer[len] = '\0';
  }

  inline auto operator=(string const &other) -> string & {
    m_buffer = other.m_buffer;
    return *this;
  }
  inline auto operator=(string &&other) -> string & {
    m_buffer = std::move(other.m_buffer);
    return *this;
  }

  inline auto operator=(char const *str) -> string & {
    m_buffer.clear();
    append(str);
    return *this;
  }

  inline auto reserve(size_type count) -> void { m_buffer.reserve(count); }

  inline auto data() -> char * { return m_buffer.data(); }
  [[nodiscard]] inline auto data() const -> char const * {
    return m_buffer.data();
  }

  [[nodiscard]] inline auto length() const -> size_type {
    return m_buffer.size() - 1;
  }
  [[nodiscard]] inline auto empty() const -> bool { return m_buffer.empty(); }

  // modifiers
  inline auto clear() -> void { m_buffer.clear(); }

  inline auto append(char const *str) -> void {
    auto const len = strlen(str);
    append(str, len);
  }
  template <typename TOtherAllocator>
  inline auto append(string<TOtherAllocator> const &other) -> void {
    append(other.data(), other.length());
  }
  inline auto append(char const *str, size_type const len) -> void {
    auto const old_size = m_buffer.size();
    m_buffer.resize(old_size + len);
    memcpy(m_buffer.data() + old_size - 1, str, len);
    m_buffer[old_size + len - 1] = '\0';
  }

  inline auto operator+=(char const *str) -> string & {
    append(str);
    return *this;
  }

  template <typename TOtherAllocator>
  inline auto operator+=(string<TOtherAllocator> const &other) -> string & {
    append(other);
    return *this;
  }

  inline auto operator+=(char const c) -> string & {
    append(&c, 1);
    return *this;
  }

private:
  vec<char, TAllocator> m_buffer;
};
} // namespace zinc