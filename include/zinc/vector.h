#pragma once

#include "allocator/prelude.h"
#include "base.h"
#include "debug.h"
#include "option.h"

namespace zinc {
template <typename TValue> struct array_view;

template <typename T, typename A = sys_allocator<T>> class vector {
public:
  // types:
  typedef T value_type;
  typedef T &reference;
  typedef const T &const_reference;
  typedef T *pointer;
  typedef const T *const_pointer;
  typedef T *iterator;
  typedef const T *const_iterator;
  typedef std::reverse_iterator<iterator> reverse_iterator;
  typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
  typedef ptrdiff difference_type;
  typedef usize size_type;

  // 23.3.11.2, construct/copy/destroy:
  vector(A &allocator);
  vector();
  vector(size_type size, A &allocator);
  explicit vector(size_type size);
  vector(size_type n, const T &val, A &allocator);
  vector(size_type n, const T &val);
  template <class InputIt> vector(InputIt first, InputIt last, A &allocator);
  template <class InputIt> vector(InputIt first, InputIt last);
  vector(std::initializer_list<T>, A &allocator);
  vector(std::initializer_list<T>);
  vector(const vector<T, A> &);
  vector(vector<T, A> &&);
  ~vector();
  auto operator=(const vector<T, A> &) -> vector<T, A> &;
  auto operator=(vector<T, A> &&) -> vector<T, A> &;
  auto operator=(std::initializer_list<T>) -> vector<T, A> &;
  void assign(size_type, const T &value);
  template <class InputIt> void assign(InputIt, InputIt);
  void assign(std::initializer_list<T>);

  // iterators:
  auto begin() noexcept -> iterator;
  auto cbegin() const noexcept -> const_iterator;
  auto end() noexcept -> iterator;
  auto cend() const noexcept -> const_iterator;
  auto rbegin() noexcept -> reverse_iterator;
  auto crbegin() const noexcept -> const_reverse_iterator;
  auto rend() noexcept -> reverse_iterator;
  auto crend() const noexcept -> const_reverse_iterator;

  // 23.3.11.3, capacity:
  [[nodiscard]] auto empty() const noexcept -> bool;
  [[nodiscard]] auto size() const noexcept -> size_type;
  [[nodiscard]] auto max_size() const noexcept -> size_type;
  [[nodiscard]] auto capacity() const noexcept -> size_type;
  void resize(size_type);
  void resize(size_type, const T &);
  void reserve(size_type);
  void shrink_to_fit();

  // element access
  auto operator[](size_type) -> reference;
  auto operator[](size_type) const -> const_reference;
  auto at(size_type) -> reference;
  auto at(size_type) const -> const_reference;
  auto front() -> reference;
  auto front() const -> const_reference;
  auto back() -> reference;
  auto back() const -> const_reference;

  // 23.3.11.4, data access:
  auto data() noexcept -> T *;
  auto data() const noexcept -> const T *;

  // 23.3.11.5, modifiers:
  template <class... Args> void emplace_back(Args &&...args);
  void push_back(const T &);
  void push_back(T &&);
  void pop_back();

  template <class... Args> auto emplace(const_iterator, Args &&...) -> iterator;
  auto insert(const_iterator, const T &) -> iterator;
  auto insert(const_iterator, T &&) -> iterator;
  auto insert(const_iterator, size_type, const T &) -> iterator;
  template <class InputIt>
  auto insert(const_iterator, InputIt, InputIt) -> iterator;
  auto insert(const_iterator, std::initializer_list<T>) -> iterator;
  auto erase(const_iterator) -> iterator;
  auto erase(const_iterator, const_iterator) -> iterator;
  void swap(vector<T, A> &);
  void clear() noexcept;

  auto operator==(const vector<T, A> &) const -> bool;
  auto operator!=(const vector<T, A> &) const -> bool;
  auto operator<(const vector<T, A> &) const -> bool;
  auto operator<=(const vector<T, A> &) const -> bool;
  auto operator>(const vector<T, A> &) const -> bool;
  auto operator>=(const vector<T, A> &) const -> bool;

  auto get_allocator() const -> A &;

  auto to_array_view() const -> array_view<T>;
  operator array_view<T>() const;
  vector(const array_view<T> &, A &allocator);
  vector(const array_view<T> &);

private:
  using alloc = std::allocator_traits<A>;

  A &m_allocator;
  size_type m_capacity = 4;
  size_type m_size = 0;
  T *m_arr;

  inline void reallocate();
};

template <typename T, typename A>
vector<T, A>::vector(A &allocator) : m_allocator(allocator) {
  m_arr = alloc::allocate(m_allocator, m_capacity);
  memset(m_arr, 0, m_capacity * sizeof(T));
}

template <typename T, typename A> vector<T, A>::vector() : vector(A()) {}

template <typename T, typename A>
vector<T, A>::vector(size_type size, A &allocator)
    : m_allocator(allocator), m_capacity(size), m_size(size) {
  m_arr = alloc::allocate(m_allocator, m_capacity);
  for (size_type i = 0; i < m_size; i++) {
    m_arr[i] = T();
  }
}

template <typename T, typename A>
vector<T, A>::vector(size_type size) : vector(size, A()) {}

template <typename T, typename A>
vector<T, A>::vector(size_type n, const T &val, A &allocator)
    : m_allocator(allocator), m_capacity(n), m_size(n) {
  m_arr = alloc::allocate(m_allocator, m_capacity);
  for (size_type i = 0; i < m_size; i++) {
    m_arr[i] = val;
  }
}

template <typename T, typename A>
vector<T, A>::vector(size_type n, const T &val) : vector(n, val, A()) {}

template <typename T, typename A>
template <class InputIt>
vector<T, A>::vector(InputIt first, InputIt last, A &allocator)
    : m_allocator(allocator) {
  size_type count = last - first;
  m_capacity = count;
  m_size = count;
  m_arr = alloc::allocate(m_allocator, m_capacity);
  for (size_type i = 0; i < m_size; i++) {
    m_arr[i] = first[i];
  }
}

template <typename T, typename A>
template <class InputIt>
vector<T, A>::vector(InputIt first, InputIt last) : vector(first, last, A()) {}

template <typename T, typename A>
vector<T, A>::vector(std::initializer_list<T> init, A &allocator)
    : m_allocator(allocator) {
  size_type count = init.size();
  m_capacity = count;
  m_arr = alloc::allocate(m_allocator, m_capacity);
  m_size = 0;
  for (auto &item : init) {
    m_arr[m_size++] = item;
  }
}

template <typename T, typename A>
vector<T, A>::vector(std::initializer_list<T> init) : vector(init, A()) {}

template <typename T, typename A>
vector<T, A>::vector(const vector<T, A> &other)
    : m_allocator(other.m_allocator), m_capacity(other.m_capacity),
      m_size(other.m_size) {
  m_arr = alloc::allocate(m_allocator, m_capacity);
  for (size_type i = 0; i < m_size; i++) {
    m_arr[i] = other.m_arr[i];
  }
}

template <typename T, typename A>
vector<T, A>::vector(vector<T, A> &&other)
    : m_allocator(other.m_allocator), m_capacity(other.m_capacity),
      m_size(other.m_size), m_arr(other.m_arr) {
  other.m_capacity = 0;
  other.m_size = 0;
  other.m_arr = nullptr;
}

template <typename T, typename A> vector<T, A>::~vector() {
  if constexpr (!std::is_trivial_v<T>)
    for (size_type i = 0; i < m_size; i++) {
      m_arr[i].~T();
    }
  alloc::deallocate(m_allocator, m_arr, m_capacity);
}

template <typename T, typename A>
auto vector<T, A>::operator=(const vector<T, A> &other) -> vector<T, A> & {
  if (this == &other) {
    return *this;
  }
  if (m_capacity < other.m_size) {
    m_capacity = other.m_size << 1;
    reallocate();
  }
  for (size_type i = 0; i < other.m_size; ++i)
    m_arr[i] = other.m_arr[i];
  m_size = other.m_size;
}

template <typename T, typename A>
auto vector<T, A>::operator=(vector<T, A> &&other) -> vector<T, A> & {
  if (this == &other) {
    return *this;
  }
  if constexpr (!std::is_trivial_v<T>)
    for (size_type i = 0; i < m_size; i++) {
      m_arr[i].~T();
    }
  alloc::deallocate(m_allocator, m_arr, m_capacity);
  m_allocator = other.m_allocator;
  m_capacity = other.m_capacity;
  m_size = other.m_size;
  m_arr = other.m_arr;
  other.m_capacity = 0;
  other.m_size = 0;
  other.m_arr = nullptr;
}

template <typename T, typename A>
auto vector<T, A>::operator=(std::initializer_list<T> ilist) -> vector<T, A> & {
  m_size = ilist.size();
  if (m_capacity < m_size) {
    m_capacity = m_size << 1;
    reallocate();
  }
  auto begin = ilist.begin();
  for (size_type i = 0; i < m_size; ++i)
    m_arr[i] = begin[i];
}

template <typename T, typename A>
auto vector<T, A>::assign(typename vector<T, A>::size_type count,
                          const T &value) -> void {
  m_size = count;
  if (m_capacity < m_size) {
    m_capacity = m_size << 1;
    reallocate();
  }
  for (size_type i = 0; i < m_size; ++i)
    m_arr[i] = value;
}

template <typename T, typename A>
template <class InputIt>
auto vector<T, A>::assign(InputIt first, InputIt last) -> void {
  m_size = last - first;
  if (m_capacity < m_size) {
    m_capacity = m_size << 1;
    reallocate();
  }
  for (size_type i = 0; i < m_size; ++i)
    m_arr[i] = first[i];
}

template <typename T, typename A>
auto vector<T, A>::assign(std::initializer_list<T> ilist) -> void {
  m_size = ilist.size();
  if (m_capacity < m_size) {
    m_capacity = ilist.size() << 1;
    reallocate();
  }
  auto begin = ilist.begin();
  for (size_type i = 0; i < ilist.size(); ++i)
    m_arr[i] = begin[i];
}

template <typename T, typename A>
auto vector<T, A>::begin() noexcept -> typename vector<T, A>::iterator {
  return m_arr;
}

template <typename T, typename A>
auto vector<T, A>::cbegin() const noexcept ->
    typename vector<T, A>::const_iterator {
  return m_arr;
}

template <typename T, typename A>
auto vector<T, A>::end() noexcept -> typename vector<T, A>::iterator {
  return m_arr + m_size;
}

template <typename T, typename A>
auto vector<T, A>::cend() const noexcept ->
    typename vector<T, A>::const_iterator {
  return m_arr + m_size;
}

template <typename T, typename A>
auto vector<T, A>::rbegin() noexcept ->
    typename vector<T, A>::reverse_iterator {
  return reverse_iterator(m_arr + m_size);
}

template <typename T, typename A>
auto vector<T, A>::crbegin() const noexcept ->
    typename vector<T, A>::const_reverse_iterator {
  return const_reverse_iterator(m_arr + m_size);
}

template <typename T, typename A>
auto vector<T, A>::rend() noexcept -> typename vector<T, A>::reverse_iterator {
  return reverse_iterator(m_arr);
}

template <typename T, typename A>
auto vector<T, A>::crend() const noexcept ->
    typename vector<T, A>::const_reverse_iterator {
  return const_reverse_iterator(m_arr);
}

template <typename T, typename A> inline void vector<T, A>::reallocate() {
  pointer new_arr = alloc::allocate(m_allocator, m_capacity);
  memset(new_arr + m_size, 0, (m_capacity - m_size) * sizeof(T));
  memcpy(new_arr, m_arr, m_size * sizeof(T));
  alloc::deallocate(m_allocator, m_arr, m_capacity);
  m_arr = new_arr;
}

template <typename T, typename A>
auto vector<T, A>::empty() const noexcept -> bool {
  return m_size == 0;
}

template <typename T, typename A>
auto vector<T, A>::size() const noexcept -> typename vector<T, A>::size_type {
  return m_size;
}

template <typename T, typename A>
auto vector<T, A>::max_size() const noexcept ->
    typename vector<T, A>::size_type {
  return std::numeric_limits<size_type>::max();
}

template <typename T, typename A>
auto vector<T, A>::capacity() const noexcept ->
    typename vector<T, A>::size_type {
  return m_capacity;
}

template <typename T, typename A>
void vector<T, A>::resize(typename vector<T, A>::size_type size) {
  if constexpr (std::is_trivial_v<T>) {
    if (size > m_capacity) {
      m_capacity = size;
      reallocate();
    }
  } else {
    if (size > m_size) {
      if (size > m_capacity) {
        m_capacity = size;
        reallocate();
      }
    } else {
      for (size_type i = m_size; i < size; ++i)
        m_arr[i].~T();
    }
    m_size = size;
  }
}

template <typename T, typename A>
void vector<T, A>::resize(typename vector<T, A>::size_type size,
                          const T &value) {
  if constexpr (std::is_trivial_v<T>) {
    if (size > m_size) {
      if (size > m_capacity) {
        m_capacity = size;
        reallocate();
      }
      for (size_type i = m_size; i < size; ++i)
        m_arr[i] = value;
    }
  } else {
    if (size > m_size) {
      if (size > m_capacity) {
        m_capacity = size;
        reallocate();
      }
      for (size_type i = m_size; i < size; ++i)
        m_arr[i] = value;
    } else {
      for (size_type i = m_size; i < size; ++i)
        m_arr[i].~T();
    }
  }
  m_size = size;
}

template <typename T, typename A>
void vector<T, A>::reserve(typename vector<T, A>::size_type capacity) {
  if (capacity > m_capacity) {
    m_capacity = capacity;
    reallocate();
  }
}

template <typename T, typename A> void vector<T, A>::shrink_to_fit() {
  m_capacity = m_size;
  reallocate();
}

template <typename T, typename A>
auto vector<T, A>::operator[](typename vector<T, A>::size_type index) ->
    typename vector<T, A>::reference {
  return m_arr[index];
}

template <typename T, typename A>
auto vector<T, A>::operator[](typename vector<T, A>::size_type index) const ->
    typename vector<T, A>::const_reference {
  return m_arr[index];
}

template <typename T, typename A>
auto vector<T, A>::at(typename vector<T, A>::size_type index) ->
    typename vector<T, A>::reference {
  ZINC_ASSERTF(index < m_size, "accessed position is out of range");
  return m_arr[index];
}

template <typename T, typename A>
auto vector<T, A>::at(typename vector<T, A>::size_type index) const ->
    typename vector<T, A>::const_reference {
  ZINC_ASSERTF(index < m_size, "accessed position is out of range");
  return m_arr[index];
}

template <typename T, typename A>
auto vector<T, A>::front() -> typename vector<T, A>::reference {
  return m_arr[0];
}

template <typename T, typename A>
auto vector<T, A>::front() const -> typename vector<T, A>::const_reference {
  return m_arr[0];
}

template <typename T, typename A>
auto vector<T, A>::back() -> typename vector<T, A>::reference {
  return m_arr[m_size - 1];
}

template <typename T, typename A>
auto vector<T, A>::back() const -> typename vector<T, A>::const_reference {
  return m_arr[m_size - 1];
}

template <typename T, typename A>
auto vector<T, A>::data() noexcept -> typename vector<T, A>::pointer {
  return m_arr;
}

template <typename T, typename A>
auto vector<T, A>::data() const noexcept ->
    typename vector<T, A>::const_pointer {
  return m_arr;
}

template <typename T, typename A>
template <class... Args>
void vector<T, A>::emplace_back(Args &&...args) {
  if (m_size == m_capacity) {
    m_capacity <<= 1;
    reallocate();
  }
  m_arr[m_size] = std::move(T(std::forward<Args>(args)...));
  ++m_size;
}

template <typename T, typename A> void vector<T, A>::push_back(const T &value) {
  if (m_size == m_capacity) {
    m_capacity <<= 1;
    reallocate();
  }
  m_arr[m_size] = value;
  ++m_size;
}

template <typename T, typename A> void vector<T, A>::push_back(T &&value) {
  if (m_size == m_capacity) {
    m_capacity <<= 1;
    reallocate();
  }
  m_arr[m_size] = std::move(value);
  ++m_size;
}

template <typename T, typename A> void vector<T, A>::pop_back() {
  --m_size;
  if constexpr (!std::is_trivial_v<T>)
    m_arr[m_size].~T();
}

template <typename T, typename A>
template <class... Args>
auto vector<T, A>::emplace(typename vector<T, A>::const_iterator pos,
                           Args &&...args) -> typename vector<T, A>::iterator {
  iterator iter = &m_arr[pos - m_arr];
  if (m_size == m_capacity) {
    m_capacity <<= 1;
    reallocate();
  }
  memmove(iter + 1, iter, (m_size - (pos - m_arr)) * sizeof(T));
  (*iter) = std::move(T(std::forward<Args>(args)...));
  ++m_size;
  return iter;
}

template <typename T, typename A>
auto vector<T, A>::insert(typename vector<T, A>::const_iterator pos,
                          const T &value) -> typename vector<T, A>::iterator {
  iterator iter = &m_arr[pos - m_arr];
  if (m_size == m_capacity) {
    m_capacity <<= 1;
    reallocate();
  }
  memmove(iter + 1, iter, (m_size - (pos - m_arr)) * sizeof(T));
  (*iter) = value;
  ++m_size;
  return iter;
}

template <typename T, typename A>
auto vector<T, A>::insert(typename vector<T, A>::const_iterator pos, T &&value)
    -> typename vector<T, A>::iterator {
  iterator iter = &m_arr[pos - m_arr];
  if (m_size == m_capacity) {
    m_capacity <<= 1;
    reallocate();
  }
  memmove(iter + 1, iter, (m_size - (pos - m_arr)) * sizeof(T));
  (*iter) = std::move(value);
  ++m_size;
  return iter;
}

template <typename T, typename A>
auto vector<T, A>::insert(typename vector<T, A>::const_iterator pos,
                          typename vector<T, A>::size_type count,
                          const T &value) -> typename vector<T, A>::iterator {
  iterator iter = &m_arr[pos - m_arr];
  if (!count)
    return iter;
  if (m_size + count > m_capacity) {
    m_capacity = m_size + (count << 1);
    reallocate();
  }
  memmove(iter + count, iter, (m_size - (pos - m_arr)) * sizeof(T));
  m_size += count;
  for (iterator i = iter; i < iter + count; ++i)
    (*i) = value;
  return iter;
}

template <typename T, typename A>
template <class InputIt>
auto vector<T, A>::insert(typename vector<T, A>::const_iterator pos,
                          InputIt first, InputIt last) ->
    typename vector<T, A>::iterator {
  iterator iter = &m_arr[pos - m_arr];
  size_type count = last - first;
  if (!count)
    return iter;
  if (m_size + count > m_capacity) {
    m_capacity = m_size + (count << 1);
    reallocate();
  }
  memmove(iter + count, iter, (m_size - (pos - m_arr)) * sizeof(T));
  m_size += count;
  for (iterator i = iter; first != last; ++i, ++first)
    (*i) = *first;
  return iter;
}

template <typename T, typename A>
auto vector<T, A>::insert(typename vector<T, A>::const_iterator pos,
                          std::initializer_list<T> ilist) ->
    typename vector<T, A>::iterator {
  iterator iter = &m_arr[pos - m_arr];
  size_type count = ilist.size();
  if (!count)
    return iter;
  if (m_size + count > m_capacity) {
    m_capacity = m_size + (count << 1);
    reallocate();
  }
  memmove(iter + count, iter, (m_size - (pos - m_arr)) * sizeof(T));
  m_size += count;
  iterator move_iter = iter;
  for (auto &item : ilist)
    (*move_iter++) = item;
  return iter;
}

template <typename T, typename A>
auto vector<T, A>::erase(typename vector<T, A>::const_iterator pos) ->
    typename vector<T, A>::iterator {
  iterator iter = &m_arr[pos - m_arr];
  if constexpr (!std::is_trivial_v<T>)
    (*iter).~T();
  memmove(iter, iter + 1, (m_size - (pos - m_arr) - 1) * sizeof(T));
  --m_size;
  return iter;
}

template <typename T, typename A>
auto vector<T, A>::erase(typename vector<T, A>::const_iterator first,
                         typename vector<T, A>::const_iterator last) ->
    typename vector<T, A>::iterator {
  iterator iter = &m_arr[first - m_arr];
  if (first == last)
    return iter;
  if constexpr (!std::is_trivial_v<T>) {
    for (; first != last; ++first)
      (*first).~T();
  }
  m_size -= last - first;
  memmove(iter, last, (m_size - (last - m_arr)) * sizeof(T));
  return iter;
}

template <typename T, typename A> void vector<T, A>::swap(vector<T, A> &other) {
  std::swap(m_arr, other.m_arr);
  std::swap(m_size, other.m_size);
  std::swap(m_capacity, other.m_capacity);
  std::swap(m_allocator, other.m_allocator);
}

template <typename T, typename A> void vector<T, A>::clear() noexcept {
  if constexpr (!std::is_trivial_v<T>)
    for (size_type i = 0; i < m_size; ++i)
      m_arr[i].~T();
  m_size = 0;
}

template <typename T, typename A>
auto vector<T, A>::operator==(const vector<T, A> &other) const -> bool {
  if (m_size != other.m_size)
    return false;
  for (size_type i = 0; i < m_size; ++i)
    if (m_arr[i] != other.m_arr[i])
      return false;
  return true;
}

template <typename T, typename A>
auto vector<T, A>::operator!=(const vector<T, A> &other) const -> bool {
  return !(*this == other);
}

template <typename T, typename A>
auto vector<T, A>::operator<(const vector<T, A> &other) const -> bool {
  size_type min_size = m_size < other.m_size ? m_size : other.m_size;
  for (size_type i = 0; i < min_size; ++i)
    if (m_arr[i] != other.m_arr[i])
      return m_arr[i] < other.m_arr[i];
  return m_size < other.m_size;
}

template <typename T, typename A>
auto vector<T, A>::operator>(const vector<T, A> &other) const -> bool {
  size_type min_size = m_size < other.m_size ? m_size : other.m_size;
  for (size_type i = 0; i < min_size; ++i)
    if (m_arr[i] != other.m_arr[i])
      return m_arr[i] > other.m_arr[i];
  return m_size > other.m_size;
}

template <typename T, typename A>
auto vector<T, A>::operator<=(const vector<T, A> &other) const -> bool {
  return !(*this > other);
}

template <typename T, typename A>
auto vector<T, A>::operator>=(const vector<T, A> &other) const -> bool {
  return !(*this < other);
}

template <typename T, typename A>
auto vector<T, A>::get_allocator() const -> A & {
  return m_allocator;
}

template <typename T> struct array_view {
  using value_type = T;
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using reference = const T &;
  using const_reference = const T &;
  using pointer = const T *;
  using const_pointer = const T *;
  using iterator = const T *;
  using const_iterator = const T *;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  array_view() = default;
  array_view(const array_view &) = default;
  array_view(array_view &&) = default;
  auto operator=(const array_view &) -> array_view & = default;
  auto operator=(array_view &&) -> array_view & = default;
  ~array_view() = default;

  array_view(const T *arr, size_type size) : m_arr(arr), m_size(size) {}

  auto operator[](size_type pos) const -> const_reference { return m_arr[pos]; }

  auto at(size_type pos) const -> const_reference {
    ZINC_ASSERTF(pos < m_size, "array_view::at")
    return m_arr[pos];
  }

  auto front() const -> const_reference { return m_arr[0]; }
  auto back() const -> const_reference { return m_arr[m_size - 1]; }
  auto data() const -> const_pointer { return m_arr; }
  auto begin() const -> const_iterator { return m_arr; }
  auto end() const -> const_iterator { return m_arr + m_size; }
  auto cbegin() const -> const_iterator { return m_arr; }
  auto cend() const -> const_iterator { return m_arr + m_size; }
  auto rbegin() const -> const_reverse_iterator { return m_arr + m_size; }
  auto rend() const -> const_reverse_iterator { return m_arr; }
  auto crbegin() const -> const_reverse_iterator { return m_arr + m_size; }
  auto crend() const -> const_reverse_iterator { return m_arr; }

  [[nodiscard]] auto empty() const -> bool { return !m_size; }
  [[nodiscard]] auto size() const -> size_type { return m_size; }
  [[nodiscard]] auto max_size() const -> size_type { return m_size; }

private:
  const T *m_arr = nullptr;
  size_type m_size = 0;
};

template <typename T, typename A>
auto vector<T, A>::to_array_view() const -> array_view<T> {
  return array_view<T>(m_arr, m_size);
}

template <typename T, typename A> vector<T, A>::operator array_view<T>() const {
  return to_array_view();
}

template <typename T, typename A>
vector<T, A>::vector(const array_view<T> &view, A &allocator)
    : vector(view.size(), allocator) {
  for (size_type i = 0; i < view.size(); ++i)
    m_arr[i] = view[i];
}

template <typename T, typename A>
vector<T, A>::vector(const array_view<T> &view) : vector(view, A()) {}
} // namespace zinc