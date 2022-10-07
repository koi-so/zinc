#include "zinc/zinc.h"
#include <iostream>

using namespace zinc;

template struct zinc::option<i32>;

struct rt {
  rt(int i) : ii(i) {}
  ~rt() { std::cout << "rt dtor" << std::endl; }
  int ii;
};

auto clean_rt(rt *r) -> void {
  delete r;
  std::cout << "clean_rt" << std::endl;
}

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

  {
    rt *n = new rt(4);
    auto x = zinc::shared<rt>(n, clean_rt);
    std::cout << x.use_count() << std::endl;
    {
      auto y = x;
      std::cout << x.use_count() << std::endl;
      {
        auto z = x;
        std::cout << x.use_count() << std::endl;
      }
      std::cout << x.use_count() << std::endl;
    }
    std::cout << x.use_count() << std::endl;
  }

  return 0;
}