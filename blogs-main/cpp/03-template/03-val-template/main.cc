#include <cstddef>
#include <iostream>

template <typename T>
T t;

template <typename T>
constexpr T t2{};

template <typename T = int>
T t3{};

template <std::size_t... values>
constexpr std::size_t array[]{values...};

int main()
{
  for (const auto& i : array<1, 2, 3, 4, 5>) std::cout << i << ' ';

  t3<> = 5;

  // t2<int>;  // 相当于 constexpr int t2 = 0;
  std::cout << std::boolalpha << std::is_same_v<decltype(t2<int>), const int> << '\n';
  std::cout << &t2<int> << &t2<int> << '\n';
}