#pragma once

#include "zinc/base.h"
#include <type_traits>

namespace zinc {
template <typename TValue> struct sys_allocator_pointer_traits {
  // we have to use these so void templates work
  using reference = std::add_lvalue_reference_t<TValue>;
  using const_reference = std::add_const_t<std::add_lvalue_reference_t<TValue>>;
};

template <typename TValue = void>
struct sys_allocator : public sys_allocator_pointer_traits<TValue> {
public:
  using value_type = TValue;
  using size_type = usize;
  using pointer = TValue *;
  using const_pointer = TValue const *;
  using difference_type = ptrdiff;

  sys_allocator() noexcept = default;
  ~sys_allocator() noexcept = default;

  template <typename TOther>
  sys_allocator(const sys_allocator<TOther> &) noexcept {}

  [[nodiscard]] auto allocate(usize size, const vptr = nullptr) -> TValue * {
    return static_cast<TValue *>(::operator new(size * sizeof(TValue)));
  }
  void deallocate(TValue *ptr, usize size) noexcept { ::delete (ptr); }

  // We have to use a template, as we can't specialize a function in C++17
  template <typename TProxy = TValue,
            typename std::enable_if_t<!std::is_void_v<TProxy>>>
  auto
  construct(pointer element,
            typename sys_allocator_pointer_traits<TProxy>::const_reference arg)
      -> void {
    new (element) TValue(arg);
  }

  void destroy(pointer element) { element->~TValue(); }

  template <typename TProxy = TValue,
            typename std::enable_if_t<!std::is_void_v<TProxy>>>
  auto address(typename sys_allocator_pointer_traits<TProxy>::reference element)
      const -> pointer {
    return &element;
  }

  template <typename TProxy = TValue,
            typename std::enable_if_t<!std::is_void_v<TProxy>>>
  auto address(typename sys_allocator_pointer_traits<TProxy>::const_reference
                   element) const -> const_pointer {
    return &element;
  }

  template <typename TOther> struct rebind {
    using other = sys_allocator<TOther>;
  };
};

template <typename TValue, typename TOther>
[[nodiscard]] auto operator==(const sys_allocator<TValue> &,
                              const sys_allocator<TOther> &) noexcept -> bool {
  return true;
}

template <typename TValue, typename TOther>
[[nodiscard]] auto operator!=(const sys_allocator<TValue> &,
                              const sys_allocator<TOther> &) noexcept -> bool {
  return false;
}
} // namespace zinc