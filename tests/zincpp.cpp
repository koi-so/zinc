#include "zinc/zinc.h"

#include <iostream>

using namespace zinc;

auto main() -> int {
  result<i32, i32> g = Ok(5);
  g = move(g.inspect_err([](i32 i) { std::cout << i << std::endl; }));
  std::cout << "mnmn:" << g.unwrap() << std::endl;

  zinc::duration s(0, 500'000'001);
  std::cout << s.checked_mul(6).value().as_seconds() << std::endl;
  return 0;
}