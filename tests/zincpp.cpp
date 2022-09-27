#include "zinc/zinc.h"

#include <iostream>

using namespace zinc;

template struct zinc::option<i32>;

auto main() -> int {
  auto data_pool = zinc::pool(sizeof(char), 1000);

  auto pool_alloc = zinc::pool_allocator<char>{data_pool};
  auto str = zinc::string("hi😂 ");
  auto m = zinc::string(std::move(str));
  // str += zinc::string("eggs", pool_alloc);
  std::cout << m.as_string_view().data() << std::endl;

  return 0;
}