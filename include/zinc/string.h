#pragma once

#include "allocator/prelude.h"
#include "base.h"
#include "debug.h"
#include "vector.h"

namespace zinc {
struct string_view;

template <typename TValue = char, typename TAllocator = sys_allocator<TValue>>
struct basic_string {
public:
  using size_type = usize;

  inline explicit basic_string(TAllocator &allocator) : m_buffer(allocator) {}
  inline basic_string() : basic_string(TAllocator()) {}
  inline explicit basic_string(size_type const count, TAllocator &allocator)
      : m_buffer(count + 1, allocator) {
    m_buffer[count] = '\0';
  }
  inline basic_string(string_view &view, TAllocator &allocator);
  inline basic_string(string_view &view) : basic_string(view, TAllocator()) {}

  inline basic_string(basic_string const &other) : m_buffer(other.m_buffer) {
    m_buffer[m_buffer.size()] = '\0';
  }
  inline basic_string(basic_string &&other)
      : m_buffer(std::move(other.m_buffer)) {}

  inline basic_string(TValue const *str, TAllocator &allocator)
      : basic_string(str, strlen(str), allocator) {}
  inline basic_string(TValue const *str)
      : basic_string(str, strlen(str), TAllocator()) {}

  inline basic_string(TValue const *str, size_type const len,
                      TAllocator &allocator)
      : m_buffer(allocator) {
    m_buffer.resize(len + 1);
    memcpy(m_buffer.data(), str, len);
    m_buffer[len] = '\0';
  }
  inline basic_string(TValue const *str, size_type const len)
      : basic_string(str, len, TAllocator()) {}

  inline auto operator=(basic_string const &other) -> basic_string & {
    auto len = other.length();
    m_buffer.resize(len + 1);
    memcpy(m_buffer.data(), other.data(), len);
    m_buffer[len] = '\0';
    return *this;
  }
  inline auto operator=(basic_string &&other) -> basic_string & {
    m_buffer = std::move(other.m_buffer);
    return *this;
  }

  inline auto operator=(TValue const *str) -> basic_string & {
    auto const len = strlen(str);
    m_buffer.resize(len + 1);
    memcpy(m_buffer.data(), str, len);
    m_buffer[len] = '\0';
    return *this;
  }

  inline auto reserve(size_type count) -> void { m_buffer.reserve(count); }

  inline auto data() -> TValue * { return m_buffer.data(); }
  [[nodiscard]] inline auto data() const -> TValue const * {
    return m_buffer.data();
  }

  [[nodiscard]] inline auto length() const -> size_type {
    return m_buffer.size() - 1;
  }
  [[nodiscard]] inline auto empty() const -> bool { return m_buffer.empty(); }

  // modifiers
  inline auto clear() -> void { m_buffer.clear(); }

  inline auto append(TValue const *str) -> void {
    auto const len = strlen(str);
    append(str, len);
  }
  template <typename TOtherAllocator>
  inline auto append(basic_string<TOtherAllocator> const &other) -> void {
    append(other.data(), other.length());
  }
  inline auto append(TValue const *str, size_type const len) -> void {
    auto const old_size = m_buffer.size();
    m_buffer.resize(old_size + len);
    memcpy(m_buffer.data() + old_size - 1, str, len);
    m_buffer[old_size + len - 1] = '\0';
  }

  inline auto operator+=(TValue const *str) -> basic_string & {
    append(str);
    return *this;
  }

  template <typename TOtherAllocator>
  inline auto operator+=(basic_string<TOtherAllocator> const &other)
      -> basic_string & {
    append(other);
    return *this;
  }

  inline auto operator+=(TValue const c) -> basic_string & {
    append(&c, 1);
    return *this;
  }

  [[nodiscard]] inline auto as_string_view() const -> string_view;
  inline operator string_view() const;

private:
  vector<TValue, TAllocator> m_buffer;
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

template <typename TValue, typename TAllocator>
inline basic_string<TValue, TAllocator>::basic_string(string_view &view,
                                                      TAllocator &allocator)
    : m_buffer(allocator) {
  m_buffer.resize(view.length() + 1);
  memcpy(m_buffer.data(), view.data(), view.length());
  m_buffer[view.length()] = '\0';
}

template <typename TValue, typename TAllocator>
inline auto basic_string<TValue, TAllocator>::as_string_view() const
    -> string_view {
  return string_view{data(), length()};
}

template <typename TValue, typename TAllocator>
inline basic_string<TValue, TAllocator>::operator string_view() const {
  return as_string_view();
}

template <typename TAllocator = sys_allocator<char>>
using allocator_string = basic_string<char, TAllocator>;

using string = allocator_string<>;
} // namespace zinc