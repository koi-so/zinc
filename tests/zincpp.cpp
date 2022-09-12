#include "zinc/zinc.h"

#include <iostream>

using namespace zinc;

auto main() -> int {
  auto t = make_tuple(1, 4, false);
  auto m = move(t);
  std::cout << get_tuple<1>(m) << " " << t.get<1>() << std::endl;

  result<i32, i32> g;
  g = ok(32);
  std::cout << g.ok() << std::endl;
  return 0;
}