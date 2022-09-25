#pragma once

#include "zinc/base.h"
#include "zinc/scoped_array.h"

namespace zinc {
struct pool {
  pool(usize granularity, usize size);
  ~pool();

  [[nodiscard]] auto get_granularity() const -> usize { return m_granularity; }
  [[nodiscard]] auto get_size() const -> usize { return m_size; }
  [[nodiscard]] auto get_used() const -> usize { return m_used; }
  [[nodiscard]] auto get_overflow() const -> usize { return m_overflow; }

  auto allocate() -> vptr;
  void deallocate(vptr block);

  template <typename TValue> auto construct() -> TValue * {
    ZINC_ASSERT(sizeof(TValue) <= m_granularity);
    auto *block = reinterpret_cast<TValue *>(allocate());
    return new (block) TValue;
  }

  template <typename TValue> void destroy(TValue *instance) {
    ZINC_ASSERT(sizeof(TValue) <= m_granularity);
    instance->~TValue();
    deallocate(instance);
  }

private:
  // checks if a pointer is from this pool
  auto is_from_pool(const vptr instance) const -> bool {
    char const *block = reinterpret_cast<char const *>(instance);
    return m_storage.get() <= block &&
           block < (m_storage.get() + m_size * m_granularity);
  }

  usize m_granularity;
  usize m_size;
  usize m_used;
  usize m_overflow;

  scoped_array<char> m_storage;
  scoped_array<vptr> m_slots;
};

template <typename TValue> struct pool_allocator {
public:
  using size_type = usize;
  using difference_type = ptrdiff_t;

  using value_type = TValue;
  using pointer = TValue *;
  using const_pointer = TValue const *;
  using reference = TValue &;
  using const_reference = TValue const &;

  template <typename TOther> struct rebind {
    using other = pool_allocator<TOther>;
  };

  // crete a pool allocator with a given a pool
  pool_allocator(pool &pool) : m_pool(&pool) {
    ZINC_ASSERT(!m_pool || sizeof(TValue) <= m_pool->get_granularity());
  }

  //! Creates a pooled allocator to the argument's pool.
  /*! If the argument has no pool, then this allocator will allocate off the
   * heap.
   */
  template <typename TOther>
  pool_allocator(pool_allocator<TOther> const &arg) : m_pool(arg.m_pool) {
    ZINC_ASSERT(!m_pool || sizeof(TValue) <= m_pool->get_granularity());
  }

  //! The largest value that can meaningfully passed to allocate.
  [[nodiscard]] auto max_size() const -> size_type { return 0xffffffff; }

  // memory is allocated depending on whether there is a pool or not
  auto allocate(size_type count, const_pointer /*hint*/ = 0) const -> pointer {
    if (count == 1 && m_pool) {
      return reinterpret_cast<TValue *>(m_pool->allocate());
    } else {
      return reinterpret_cast<TValue *>(new char[count * sizeof(TValue)]);
    }
  }

  //! Deallocates memory allocated by allocate.
  void deallocate(pointer block, size_type count) const noexcept {
    ZINC_ASSERTF(block, "null pointer argument");
    if (count == 1 && m_pool) {
      m_pool->deallocate(block);
    } else {
      delete[] reinterpret_cast<char *>(block);
    }
  }

  void construct(pointer element, TValue const &arg) {
    new (element) TValue(arg);
  }

  void destroy(pointer element) { element->~TValue(); }

  auto address(reference element) const -> pointer { return &element; }
  auto address(const_reference element) const -> const_pointer {
    return &element;
  }

  //! The pool for this allocator.
  pool *m_pool;
};

// pool_allocator void specialisation
template <> struct pool_allocator<void> {
public:
  using size_type = usize;
  using difference_type = ptrdiff_t;

  using value_type = void;
  using pointer = void *;
  using const_pointer = void const *;

  template <typename TOther> struct rebind {
    using other = pool_allocator<TOther>;
  };

  pool_allocator() : m_pool(nullptr) {}
  pool_allocator(pool *pool) : m_pool(pool) {}

private:
  pool *m_pool;
};

template <typename TValue, typename TOther>
auto operator==(pool_allocator<TValue> const &left,
                pool_allocator<TOther> const &right) -> bool {
  return left.m_pool == right.m_pool;
}

template <typename TValue, typename TOther>
auto operator!=(pool_allocator<TValue> const &left,
                pool_allocator<TOther> const &right) -> bool {
  return left.m_pool != right.m_pool;
}
} // namespace zinc