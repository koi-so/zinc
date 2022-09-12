#include "zinc/zinc.h"

#include <iostream>

auto main() -> int {
  auto t = zinc::make_tuple(1, 4, false);
  auto m = move(t);
  std::cout << zinc::get_tuple<1>(m) << " " << t.get<1>() << std::endl;
  return 0;
}