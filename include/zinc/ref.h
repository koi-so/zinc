#pragma once

#include "zinc/base.h"

namespace zinc {
template <typename T, typename Traits> class ref_base {
public:
  // Default constructor and destructor.
  ref_base() : m_value(Traits::Null) {}

  ~ref_base() { release(m_value); }

  // Constructors from nullptr.
  // NOLINTNEXTLINE(runtime/explicit)
  constexpr ref_base(std::nullptr_t) : ref_base() {}

  auto operator=(std::nullptr_t) -> ref_base<T, Traits> & {
    set(Traits::Null);
    return *this;
  }

  // Constructors from a value T.
  // NOLINTNEXTLINE(runtime/explicit)
  ref_base(T value) : m_value(value) { reference(value); }

  auto operator=(const T &value) -> ref_base<T, Traits> & {
    set(value);
    return *this;
  }

  // Constructors from a ref_base<T>
  ref_base(const ref_base<T, Traits> &other) : m_value(other.m_value) {
    reference(other.m_value);
  }

  auto operator=(const ref_base<T, Traits> &other) -> ref_base<T, Traits> & {
    set(other.m_value);
    return *this;
  }

  ref_base(ref_base<T, Traits> &&other) { m_value = other.detach(); }

  auto operator=(ref_base<T, Traits> &&other) -> ref_base<T, Traits> & {
    if (&other != this) {
      release(m_value);
      m_value = other.detach();
    }
    return *this;
  }

  // Constructors from a ref_base<U>. Note that in the *-assignment operators
  // this cannot be the same as `other` because overload resolution rules would
  // have chosen the *-assignement operators defined with `other` == ref_base<T,
  // Traits>.
  template <typename U, typename UTraits,
            typename = typename std::is_convertible<U, T>::type>
  ref_base(const ref_base<U, UTraits> &other) : m_value(other.m_value) {
    reference(other.m_value);
  }

  template <typename U, typename UTraits,
            typename = typename std::is_convertible<U, T>::type>
  ref_base<T, Traits> &operator=(const ref_base<U, UTraits> &other) {
    set(other.m_value);
    return *this;
  }

  template <typename U, typename UTraits,
            typename = typename std::is_convertible<U, T>::type>
  ref_base(ref_base<U, UTraits> &&other) {
    m_value = other.detach();
  }

  template <typename U, typename UTraits,
            typename = typename std::is_convertible<U, T>::type>
  auto operator=(ref_base<U, UTraits> &&other) -> ref_base<T, Traits> & {
    release(m_value);
    m_value = other.detach();
    return *this;
  }

  // Comparison operators.
  auto operator==(const T &other) const -> bool { return m_value == other; }

  auto operator!=(const T &other) const -> bool { return m_value != other; }

  auto operator->() const -> const T { return m_value; }
  auto operator->() -> T { return m_value; }

  // Smart pointer methods.
  auto get() const -> const T & { return m_value; }
  auto get() -> T & { return m_value; }

  [[nodiscard]] auto detach() -> T {
    T value{std::move(m_value)};
    m_value = Traits::Null;
    return value;
  }

  void acquire(T value) {
    release(m_value);
    m_value = value;
  }

  [[nodiscard]] auto init_into() -> T * {
    ZINC_ASSERT(m_value == Traits::Null);
    return &m_value;
  }

private:
  // Friend is needed so that instances of ref_base<U> can call reference and
  // release on ref_base<T>.
  template <typename U, typename UTraits> friend class ref_base;

  static void reference(T value) {
    if (value != Traits::Null) {
      Traits::reference(value);
    }
  }
  static void release(T value) {
    if (value != Traits::Null) {
      Traits::release(value);
    }
  }

  void set(T value) {
    if (m_value != value) {
      // Ensure that the new value is referenced before the old is released to
      // prevent any transitive frees that may affect the new value.
      reference(value);
      release(m_value);
      m_value = value;
    }
  }

  T m_value;
};

// template <typename T> struct RefCountedTraits {
// static constexpr T *kNullValue = nullptr;
// static void reference(T *value) { value->reference(); }
// static void release(T *value) { value->release(); }
// };
} // namespace zinc