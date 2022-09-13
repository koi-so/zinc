#pragma once

#include "base.h"

namespace zinc {
ZINC_API struct Unit {
  constexpr Unit() = default;
  constexpr Unit(const Unit &) = default;
  constexpr Unit(Unit &&) = default;
  constexpr auto operator=(const Unit &) -> Unit & = default;
  constexpr auto operator=(Unit &&) -> Unit & = default;
  constexpr auto operator==(const Unit &) const -> bool { return true; }
  constexpr auto operator!=(const Unit &) const -> bool { return false; }
  constexpr auto operator<(const Unit &) const -> bool { return false; }
  constexpr auto operator<=(const Unit &) const -> bool { return true; }
  constexpr auto operator>(const Unit &) const -> bool { return false; }
  constexpr auto operator>=(const Unit &) const -> bool { return true; }
};
} // namespace zinc