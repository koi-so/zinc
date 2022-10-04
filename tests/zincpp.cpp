#include "zinc/zinc.h"

#include <iostream>

using namespace zinc;

template struct zinc::option<i32>;

struct m {
  zinc::string s;
};

auto main() -> int {
  auto data_pool = zinc::pool(sizeof(char), 1000);

  auto pool_alloc = zinc::pool_allocator<char>{data_pool};

  auto str = zinc::string();
  auto x = m{};
  x.s = zinc::string("hello 😁");
  // str += zinc::string("eggs", pool_alloc);
  std::cout << x.s.data() << std::endl;

  return 0;
}