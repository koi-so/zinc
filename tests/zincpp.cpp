#include "zinc/zinc.h"

#include <iostream>

using namespace zinc;

template struct zinc::option<i32>;

struct m {
  m(int i) : ii(i) {}
  int ii;
};

auto main() -> int {
  auto data_pool = zinc::pool(sizeof(char), 1000);

  auto dyna = zinc::string("Hello World! 😱");
  std::cout << dyna.data() << std::endl;

  auto m = zinc::vector<int>();
  m.push_back(6);
  m.push_back(7);
  m.push_back(8);

  for (auto i : m) {
    std::cout << i << std::endl;
  }

  return 0;
}