#pragma once

#include "base.h"
#include "debug.h"

namespace zinc {
// A scoped array is a wrapper around a pointer to an array of elements.
template <typename TValue> struct scoped_array : non_copyable {
public:
  using element_type = TValue;

  explicit scoped_array(TValue *pointer = nullptr) : m_pointer(pointer) {}
  ~scoped_array() { checked_array_delete(m_pointer); }

  void reset(TValue *pointer = nullptr) {
    ZINC_ASSERT(pointer == nullptr || pointer != m_pointer);
    this_type(pointer).swap(*this);
  }

  auto operator[](ptrdiff index) const noexcept -> TValue & {
    ZINC_ASSERT(m_pointer != nullptr);
    ZINC_ASSERT(index > 0);
    return m_pointer[index];
  }
  auto get() const noexcept -> TValue * { return m_pointer; }

  operator bool() const noexcept { return m_pointer != nullptr; }

  void swap(scoped_array &other) noexcept {
    std::swap(m_pointer, other.m_pointer);
  }

private:
  using this_type = scoped_array<TValue>;
  TValue *m_pointer;
};

template <class TValue>
inline auto operator==(scoped_array<TValue> const &p, std::nullptr_t) noexcept
    -> bool {
  return p.get() == nullptr;
}

template <class TValue>
inline auto operator==(std::nullptr_t, scoped_array<TValue> const &p) noexcept
    -> bool {
  return p.get() == nullptr;
}

template <class TValue>
inline auto operator!=(scoped_array<TValue> const &p, std::nullptr_t) noexcept
    -> bool {
  return p.get() != nullptr;
}

template <class TValue>
inline auto operator!=(std::nullptr_t, scoped_array<TValue> const &p) noexcept
    -> bool {
  return p.get() != nullptr;
}
} // namespace zinc

namespace std {
template <typename TValue>
inline void swap(zinc::scoped_array<TValue> &a,
                 zinc::scoped_array<TValue> &b) noexcept {
  a.swap(b);
}
} // namespace std