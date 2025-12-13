#include <concepts>
#include <ios>
#include <iostream>

template <typename T>
concept Addable = requires(T t) { t + t; };

template <Addable T>
auto add(T a, T b) -> decltype(a + b)
{
  return a + b;
}

template <std::floating_point T>
void f(T)
{
}

int main()
{
  std::cout << add(1, 2) << '\n';
  f(1.);
  std::cout << std::boolalpha << std::convertible_to<int, double>;
}