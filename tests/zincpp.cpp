#include "zinc/zinc.h"

#include <iostream>

using namespace zinc;

auto main() -> int {
  result<i32, i32> g = Ok(5);
  g = move(g.inspect_err([](i32 i) { std::cout << i << std::endl; }));
  std::cout << "mnmn:" << g.unwrap() << std::endl;

  zinc::duration s(5, 60);
  std::cout << s.as_microseconds() << std::endl;
  return 0;
}