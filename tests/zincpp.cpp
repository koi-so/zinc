#include "zinc/zinc.h"

#include <iostream>

using namespace zinc;

template struct zinc::option<i32>;

struct m {
  virtual void f() = 0;
  int ii;
};

struct n : m {
  n(int r) : ii(r) {}
  void f() override { std::cout << "n::f() " << ii << std::endl; }
  int ii;
};

auto main() -> int {
  auto data_pool = zinc::pool(sizeof(char), 1000);

  auto n1 = new n{3};
  auto n2 = new n{4};

  auto dyna = zinc::dyn_array<n *>{};
  dyna.push_back(n1);
  dyna.push_back(n2);
  auto dyna2 = zinc::dyn_array<m *>{};
  dyna2 = dyna;

  std::cout << dyna.size() << " " << dyna2.size() << std::endl;

  for (auto item : dyna2) {
    std::cout << item << std::endl;
    item->f();
  }

  return 0;
}