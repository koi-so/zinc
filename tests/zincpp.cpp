#include "zinc/zinc.h"

#include <iostream>

using namespace zinc;

auto main() -> int {
  result<i32, i32> g = Err(5);
  g = g.inspect_err([](i32 i) { std::cout << i << std::endl; });
  return 0;
}