#include "zinc/zinc.h"

#include <iostream>

using namespace zinc;

template struct zinc::option<i32>;

auto main() -> int {
  auto data_pool = zinc::pool(sizeof(char), 1000);

  auto pool_alloc = zinc::pool_allocator<char>{data_pool};
  auto str = zinc::string<zinc::pool_allocator<char>>{"hi😂 ", pool_alloc};
  str += zinc::string("eggs");
  std::cout << str.data() << std::endl;

  return 0;
}