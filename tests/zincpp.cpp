#include "zinc/zinc.h"

#include <iostream>

using namespace zinc;

template struct zinc::option<i32>;

auto main() -> int {
  option<i32> g = Some(2);
  g = g.map<i32>([](i32 const &x) { return x + 1; });
  g = g.map<i32>([](i32 const &x) { return x + 1; });
  g = g.map<i32>([](i32 const &x) { return x + 1; });
  std::cout << g.value() << std::endl;

  option<i64> f = Some(16);
  std::cout << (f == g) << std::endl;

  zinc::duration s(0, 500'000'001);
  std::cout << s.checked_mul(6).value().as_seconds_floating<f64>() << std::endl;

  auto talloc = zinc::heap_allocator<i32>{};
  i32 *p = talloc.allocate(1);
  *p = 43;
  std::cout << *p << std::endl;
  talloc.deallocate(p, 1);
  return 0;
}