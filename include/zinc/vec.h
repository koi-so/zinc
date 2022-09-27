#pragma once

#include "allocator/prelude.h"
#include "base.h"
#include "debug.h"
#include "option.h"

namespace zinc {
template <class TValue>
inline void construct_range(TValue *begin, TValue *end) {
  while (begin != end) {
    new (begin) TValue;
    begin++;
  }
}

template <class TValue>
inline void copy_range(TValue *begin, TValue *end, TValue *dest) {
  while (begin != end) {
    new (dest) TValue(*begin);
    begin++;
    dest++;
  }
}

template <class TValue> inline void destruct_range(TValue *begin, TValue *end) {
  while (begin != end) {
    begin->~T();
    begin++;
  }
}

template <typename TValue, typename TAllocator = sys_allocator<TValue>>
struct vec {
private:
  using rebind_allocator =
      typename std::allocator_traits<TAllocator>::template rebind_alloc<TValue>;
  using allocator_traits = std::allocator_traits<rebind_allocator>;

public:
  using value_type = TValue;
  using allocator_type = TAllocator;
  using pointer = typename allocator_traits::pointer;
  using const_pointer = typename allocator_traits::const_pointer;
  using reference = TValue &;
  using const_reference = TValue const &;
  using size_type = typename allocator_traits::size_type;
  using difference_type = typename allocator_traits::difference_type;

  inline explicit vec(TAllocator &allocator) : m_allocator(allocator) {}

  inline explicit vec(size_type const count, TAllocator &allocator)
      : m_allocator(allocator), m_size(count), m_capacity(count) {
    m_allocator = allocator;
    m_data = allocator_traits::allocate(m_allocator, m_capacity);

    if constexpr (std::is_trivial_v<TValue>) {
      std::memset(m_data, 0, m_capacity * sizeof(TValue));
    } else {
      construct_range(m_data, m_data + m_size);
    }
  }

  inline vec(vec const &other)
      : m_allocator(other.m_allocator), m_size(other.m_size),
        m_capacity(other.m_capacity) {
    m_data = allocator_traits::allocate(m_allocator, m_size);
    if constexpr (std::is_trivial_v<TValue>) {
      std::memcpy(m_data, other.m_data, m_capacity * sizeof(TValue));
    } else {
      copy_range(other.m_data, other.m_data + m_size, m_data);
    }
  }
  inline vec(vec &&other) noexcept
      : m_allocator(other.m_allocator), m_data(other.m_data),
        m_size(other.m_size), m_capacity(other.m_capacity) {
    other.m_data = nullptr;
    other.m_size = 0;
    other.m_capacity = 0;
  }
  inline auto operator=(vec const &other) -> vec & {
    m_allocator = other.m_allocator;
    m_size = other.m_size;
    m_capacity = other.m_capacity;

    m_data = allocator_traits::allocate(m_allocator, m_capacity);
    if constexpr (std::is_trivial_v<TValue>) {
      std::memcpy(m_data, other.m_data, m_capacity * sizeof(TValue));
    } else {
      copy_range(other.m_data, other.m_data + m_size, m_data);
    }

    return *this;
  }
  inline auto operator=(vec &&other) noexcept -> vec & {
    m_allocator = other.m_allocator;
    m_data = other.m_data;
    m_size = other.m_size;
    m_capacity = other.m_capacity;

    other.m_data = nullptr;
    other.m_size = 0;
    other.m_capacity = 0;

    return *this;
  }

  inline ~vec() {
    if (m_data) {
      if constexpr (!std::is_trivial_v<TValue>) {
        destruct_range(m_data, m_data + m_size);
      }
      allocator_traits::deallocate(m_allocator, m_data, m_capacity);
    }
  }

  inline auto operator[](size_type position) -> TValue & {
    ZINC_ASSERTF(position < m_size, "Index out of range");
    return m_data[position];
  }
  inline auto operator[](size_type position) const -> TValue const & {
    ZINC_ASSERTF(position < m_size, "Index out of range");
    return m_data[position];
  }

  inline auto get(size_type position) -> option<TValue> {
    if (position < m_size) {
      return m_data[position];
    } else {
      return None;
    }
  }
  inline auto get(size_type position) const -> option<TValue const> {
    if (position < m_size) {
      return m_data[position];
    } else {
      return None;
    }
  }

  inline auto front() -> TValue & {
    ZINC_ASSERTF(m_size > 0, "Vec is empty");
    return m_data[0];
  }
  inline auto front() const -> TValue const & {
    ZINC_ASSERTF(m_size > 0, "Vec is empty");
    return m_data[0];
  }

  inline auto back() -> TValue & {
    ZINC_ASSERTF(m_size > 0, "Vec is empty");
    return m_data[m_size - 1];
  }
  inline auto back() const -> TValue const & {
    ZINC_ASSERTF(m_size > 0, "Vec is empty");
    return m_data[m_size - 1];
  }

  inline auto data() -> TValue * { return m_data; }
  inline auto data() const -> TValue const * { return m_data; }

  inline auto begin() -> TValue * { return m_data; }
  inline auto begin() const -> TValue const * { return m_data; }

  inline auto end() -> TValue * { return m_data + m_size; }
  inline auto end() const -> TValue const * { return m_data + m_size; }

  [[nodiscard]] inline auto empty() const noexcept -> bool {
    return m_size == 0;
  }
  [[nodiscard]] inline auto size() const noexcept -> size_type {
    return m_size;
  }
  inline void reserve(size_type new_capacity) {
    ZINC_ASSERTF(new_capacity >= m_capacity, "New capacity must be >= size");

    auto new_data = allocator_traits::allocate(m_allocator, new_capacity);
    ZINC_ASSERTF(new_data != nullptr, "Failed to allocate");

    if constexpr (std::is_trivial_v<TValue>) {
      std::memcpy(new_data, m_data, m_size * sizeof(TValue));
    } else {
      copy_range(begin(), end(), new_data);
      destruct_range(begin(), end());
    }

    allocator_traits::deallocate(m_allocator, m_data, m_capacity);
    m_data = new_data;
    m_capacity = new_capacity;
  }
  [[nodiscard]] inline auto capacity() const noexcept -> size_type {
    return m_capacity;
  }
  inline void shrink_to_fit() {
    if (m_size < m_capacity) {
      auto new_data = allocator_traits::allocate(m_allocator, m_size);
      ZINC_ASSERTF(new_data != nullptr, "Failed to allocate");

      if constexpr (std::is_trivial_v<TValue>) {
        std::memcpy(new_data, m_data, m_size * sizeof(TValue));
      } else {
        copy_range(begin(), end(), new_data);
        destruct_range(begin(), end());
      }

      allocator_traits::deallocate(m_allocator, m_data, m_capacity);
      m_data = new_data;
      m_capacity = m_size;
    }
  }

  inline void clear() noexcept {
    if constexpr (!std::is_trivial_v<TValue>) {
      destruct_range(begin(), end());
    }
    m_size = 0;
  }

  inline void push_back(TValue const &value) {
    if (m_size == m_capacity) {
      reserve(m_capacity * grow_factor + 1);
    }
    if constexpr (std::is_trivial_v<TValue>) {
      m_data[m_size] = value;
    } else {
      new (m_data + m_size) TValue(value);
    }
    m_size++;
  }
  inline void push_back(TValue &&value) {
    if (m_size == m_capacity) {
      reserve(m_capacity * grow_factor + 1);
    }
    if constexpr (std::is_trivial_v<TValue>) {
      m_data[m_size] = value;
    } else {
      new (m_data + m_size) TValue(std::move(value));
    }
    m_size++;
  }

  template <typename... TArgs> inline void emplace_back(TArgs &&...args) {
    static_assert(!std::is_trivial_v<TValue>,
                  "Use push_back for trivial types");
    if (m_size == m_capacity) {
      reserve(m_capacity * grow_factor + 1);
    }
    new (m_data + m_size) TValue(std::forward<TArgs>(args)...);
    m_size++;
  }

  inline void pop_back() {
    ZINC_ASSERTF(m_size > 0, "Vec is empty");
    if constexpr (!std::is_trivial_v<TValue>) {
      m_data[m_size - 1].~TValue();
    }
    m_size--;
  }

  inline void resize(size_type count) {
    ZINC_ASSERTF(count != m_size, "Size is already the passed size");

    if (count > m_capacity) {
      reserve(count);
    }

    if constexpr (!std::is_trivial_v<TValue>) {
      if (count > m_size) {
        construct_range(m_data + m_size, m_data + count);
      } else if (count < m_size) {
        destruct_range(m_data + count, m_data + m_size);
      }
    }

    m_size = count;
  }

  static constexpr size_type grow_factor = 2;

private:
  TAllocator &m_allocator;
  TValue *m_data = nullptr;
  size_type m_size = 0;
  size_type m_capacity = 0;
};
} // namespace zinc