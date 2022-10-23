#pragma once

#include "base.h"

namespace zinc {
namespace details {
template <usize I, typename T> struct tuple_unit {
  template <typename U> tuple_unit(U &&u) : m_unit(std::forward<U>(u)) {}

  auto get() -> T & { return m_unit; }
  auto get() const -> const T & { return m_unit; }

private:
  T m_unit;
};

template <typename, typename...> struct tuple_base;

template <usize... Is, typename... Ts>
struct tuple_base<std::index_sequence<Is...>, Ts...> : tuple_unit<Is, Ts>... {
  template <typename... Us> tuple_base(Us... us) : tuple_unit<Is, Ts>(us)... {}
};

template <std::size_t I, typename T, typename... Ts> struct nth_element_impl {
  using type = typename nth_element_impl<I - 1, Ts...>::type;
};

template <typename T, typename... Ts> struct nth_element_impl<0, T, Ts...> {
  using type = T;
};

template <std::size_t I, typename... Ts>
using nth_element = typename nth_element_impl<I, Ts...>::type;

} // namespace details

template <typename... Ts>
class tuple
    : public details::tuple_base<std::make_index_sequence<sizeof...(Ts)>,
                                 Ts...> {
public:
  tuple(const Ts &...ts)
      : details::tuple_base<std::make_index_sequence<sizeof...(Ts)>, Ts...>(
            ts...) {}

  // tuple(Ts &&...ts)
  //     : details::tuple_base<std::make_index_sequence<sizeof...(Ts)>, Ts...>(
  //           std::forward<Ts>(ts)...) {}

  template <usize I> auto get() -> details::nth_element<I, Ts...> & {
    return static_cast<
               details::tuple_unit<I, details::nth_element<I, Ts...>> *>(this)
        ->get();
  }
};

template <usize index, typename... TOtherElements>
auto get_tuple(tuple<TOtherElements...> &t) {
  return (static_cast<details::tuple_unit<
              index, details::nth_element<index, TOtherElements...>> &>(t))
      .get();
}

template <usize index, typename... TElements>
auto compare_tuple(tuple<TElements...> &a, tuple<TElements...> &b) -> bool {
  if constexpr (index == 0) {
    return get_tuple<0>(a) == get_tuple<0>(b);
  } else {
    return get_tuple<index>(a) == get_tuple<index>(b) &&
           compare_tuple<index - 1>(a, b);
  }
}

template <typename... TElements>
auto operator==(tuple<TElements...> &a, tuple<TElements...> &b) -> bool {
  return compare_tuple<sizeof...(TElements) - 1>(a, b);
}

template <typename... TElements>
auto operator!=(tuple<TElements...> &a, tuple<TElements...> &b) -> bool {
  return !(a == b);
}

template <typename... TElements>
auto make_tuple(TElements &&...elements) -> tuple<TElements...> {
  return tuple<TElements...>(std::forward<TElements>(elements)...);
}

template <typename... TElements>
tuple(TElements... args) -> tuple<TElements...>;

template <typename... Args>
constexpr auto tie(Args &...args) noexcept -> tuple<Args &...> {
  return {args...};
}

} // namespace zinc