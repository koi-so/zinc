#pragma once

#include "../base.h"

namespace zinc {
template <typename TValue> struct heap_allocator_pointer_traits {
  using reference = TValue &;
  using const_reference = const TValue &;
};

// Avoid declaring a reference to void with an empty specialization
template <> struct heap_allocator_pointer_traits<void> {};

template <typename TValue = void>
struct heap_allocator : public heap_allocator_pointer_traits<TValue> {
public:
  using value_type = TValue;
  using size_type = usize;
  using pointer = TValue *;
  using const_pointer = const TValue *;
  using difference_type = ptrdiff;

  heap_allocator() noexcept = default;
  ~heap_allocator() noexcept = default;

  template <typename TOther>
  heap_allocator(const heap_allocator<TOther> &) noexcept {}

  [[nodiscard]] auto allocate(usize size, const vptr = nullptr) -> TValue * {
    return static_cast<TValue *>(::operator new(size * sizeof(TValue)));
  }
  void deallocate(TValue *ptr, usize size) noexcept { ::delete (ptr); }

  template <typename TOther> struct rebind {
    using other = heap_allocator<TOther>;
  };
};

template <typename TValue, typename TOther>
[[nodiscard]] auto operator==(const heap_allocator<TValue> &,
                              const heap_allocator<TOther> &) noexcept -> bool {
  return true;
}

template <typename TValue, typename TOther>
[[nodiscard]] auto operator!=(const heap_allocator<TValue> &,
                              const heap_allocator<TOther> &) noexcept -> bool {
  return false;
}
} // namespace zinc