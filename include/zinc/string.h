#pragma once

#include "allocator/prelude.h"
#include "base.h"
#include "debug.h"
#include "vec.h"

namespace zinc {
struct string_view;

template <typename TAllocator = sys_allocator<char>> struct string {
public:
  using size_type = usize;

  inline explicit string(TAllocator const &allocator) : m_buffer(allocator) {}
  inline string() : string(TAllocator()) {}
  inline explicit string(size_type const count, TAllocator const &allocator)
      : m_buffer(count + 1, allocator) {
    m_buffer[count] = '\0';
  }
  inline string(string_view &view, TAllocator const &allocator);
  inline string(string_view &view) : string(view, TAllocator()) {}

  inline string(string const &other) : m_buffer(other.m_buffer) {}
  inline string(string &&other) : m_buffer(std::move(other.m_buffer)) {}

  inline string(char const *str, TAllocator &allocator)
      : string(str, strlen(str), allocator) {}
  inline string(char const *str) : string(str, strlen(str), TAllocator()) {}

  inline string(char const *str, size_type const len, TAllocator &allocator)
      : m_buffer(allocator) {
    m_buffer.resize(len + 1);
    memcpy(m_buffer.data(), str, len);
    m_buffer[len] = '\0';
  }
  inline string(char const *str, size_type const len)
      : string(str, len, TAllocator()) {}

  inline auto operator=(string const &other) -> string & {
    auto len = other.length();
    m_buffer.resize(len + 1);
    memcpy(m_buffer.data(), other.data(), len);
    m_buffer[len] = '\0';
    return *this;
  }
  inline auto operator=(string &&other) -> string & {
    m_buffer = std::move(other.m_buffer);
    return *this;
  }

  inline auto operator=(char const *str) -> string & {
    auto const len = strlen(str);
    m_buffer.resize(len + 1);
    memcpy(m_buffer.data(), str, len);
    m_buffer[len] = '\0';
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

  [[nodiscard]] inline auto as_string_view() const -> string_view;
  inline operator string_view() const;

private:
  vec<char, TAllocator> m_buffer;
};

struct string_view {
public:
  using size_type = usize;

  inline string_view() = default;
  inline string_view(char const *str) : m_str(str), m_len(strlen(str)) {}
  inline string_view(char const *str, size_type const len)
      : m_str(str), m_len(len) {}

  [[nodiscard]] inline auto data() const -> char const * { return m_str; }
  [[nodiscard]] inline auto length() const -> size_type { return m_len; }
  [[nodiscard]] inline auto empty() const -> bool { return m_len == 0; }

  inline auto operator[](size_type const index) const -> char {
    ZINC_ASSERT(index < m_len);
    return m_str[index];
  }

  inline auto operator==(string_view const &other) const -> bool {
    if (m_len != other.m_len) {
      return false;
    }
    return memcmp(m_str, other.m_str, m_len) == 0;
  }

  inline auto operator!=(string_view const &other) const -> bool {
    return !(*this == other);
  }

private:
  char const *m_str{nullptr};
  size_type m_len{0};
};

template <typename TAllocator>
inline string<TAllocator>::string(string_view &view,
                                  TAllocator const &allocator)
    : m_buffer(allocator) {
  m_buffer.resize(view.length() + 1);
  memcpy(m_buffer.data(), view.data(), view.length());
  m_buffer[view.length()] = '\0';
}

template <typename TAllocator>
inline auto string<TAllocator>::as_string_view() const -> string_view {
  return string_view{data(), length()};
}

template <typename TAllocator>
inline string<TAllocator>::operator string_view() const {
  return as_string_view();
}
} // namespace zinc