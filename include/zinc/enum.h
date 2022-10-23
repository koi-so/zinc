#pragma once

#include "base.h"

#define ZINC_ENABLE_BITMASK_OPERATORS(x)                                       \
  template <> struct EnableBitMaskOperators<x> {                               \
    static const bool enable = true;                                           \
  };

template <typename E> struct EnableBitMaskOperators {
  static const bool enable = false;
};

template <typename E>
inline auto operator|(E lhs, E rhs)
    -> std::enable_if_t<EnableBitMaskOperators<E>::enable, E> {
  return static_cast<E>(static_cast<std::underlying_type_t<E>>(lhs) |
                        static_cast<std::underlying_type_t<E>>(rhs));
}

template <typename E>
inline auto operator&(E lhs, E rhs)
    -> std::enable_if_t<EnableBitMaskOperators<E>::enable, E> {
  return static_cast<E>(static_cast<std::underlying_type_t<E>>(lhs) &
                        static_cast<std::underlying_type_t<E>>(rhs));
}

template <typename E>
inline auto operator^(E lhs, E rhs)
    -> std::enable_if_t<EnableBitMaskOperators<E>::enable, E> {
  return static_cast<E>(static_cast<std::underlying_type_t<E>>(lhs) ^
                        static_cast<std::underlying_type_t<E>>(rhs));
}

template <typename E>
inline auto operator~(E rhs)
    -> std::enable_if_t<EnableBitMaskOperators<E>::enable, E> {
  return static_cast<E>(~static_cast<std::underlying_type_t<E>>(rhs));
}

template <typename E>
inline auto operator|=(E &lhs, E rhs)
    -> std::enable_if_t<EnableBitMaskOperators<E>::enable, E> & {
  lhs = static_cast<E>(static_cast<std::underlying_type_t<E>>(lhs) |
                       static_cast<std::underlying_type_t<E>>(rhs));
  return lhs;
}

template <typename E>
inline auto operator&=(E &lhs, E rhs)
    -> std::enable_if_t<EnableBitMaskOperators<E>::enable, E> & {
  lhs = static_cast<E>(static_cast<std::underlying_type_t<E>>(lhs) &
                       static_cast<std::underlying_type_t<E>>(rhs));
  return lhs;
}

template <typename E>
inline auto operator^=(E &lhs, E rhs)
    -> std::enable_if_t<EnableBitMaskOperators<E>::enable, E> & {
  lhs = static_cast<E>(static_cast<std::underlying_type_t<E>>(lhs) ^
                       static_cast<std::underlying_type_t<E>>(rhs));
  return lhs;
}