#include <bit>
#include <iostream>

bool is_little_endian() {
  int num = 1;
  return *(reinterpret_cast<char*>(&num)) == 1;
}

void test1() {
  auto res = static_cast<int>(std::endian());
  if (res == 1) {
    std::cout << "Little-endian\n";
  } else {
    std::cout << "Big-endian\n";
  }
}

int main() {
  if (is_little_endian()) {
    std::cout << "Little-endian\n";
  } else {
    std::cout << "Big-endian\n";
  }
}