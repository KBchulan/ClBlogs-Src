#include "generator.hpp"
#include <iostream>

Generator<int> range(int start, int end) {
  for (int i = start; i < end; ++i) {
    co_yield i;
  }
}

int main() {
  auto gen = range(1, 10);
  while (gen.next()) {
    std::cout << gen.value() << " ";
  }
}