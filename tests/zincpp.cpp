#include "zinc/zinc.h"

#include <iostream>

using namespace zinc;

template struct zinc::option<i32>;

auto main() -> int {
  auto data_pool = zinc::pool(sizeof(i32), 1000);
  auto pool_alloc = zinc::pool_allocator<i32>{data_pool};

  auto v = zinc::vec<i32, zinc::pool_allocator<i32>>{pool_alloc};
  v.push_back(1);
  v.push_back(2);
  v.push_back(3);

  v.pop_back();

  for (auto &x : v) {
    std::cout << x << std::endl;
  }

  auto r = v.get(1);
  std::cout << r.has_value() << std::endl;

  return 0;
}