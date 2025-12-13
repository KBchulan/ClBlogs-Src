#include <cstddef>
#include <iostream>

template <typename T>
constexpr const char* x = "?";

template <typename T>
constexpr const char* x<T*> = "pointer";

template <typename T, typename U>
constexpr const char* Y = "?";

template <typename T>
constexpr const char* Y<int, T> = "int-specialized";

template <typename T, std::size_t N>
struct X1
{
  template <typename T1, typename T2>
  struct X2
  {
  };
};

template <>
template <typename T2>
struct X1<int, 10>::X2<int, T2>
{
  void f()
  {
    std::cout << "X1<int, 10>::X2<int, T2>::f()\n";
  }
};

template <typename, typename>
constexpr bool is_same_v = false;

template <typename T>
constexpr bool is_same_v<T, T> = true;

int main()
{
  std::cout << std::boolalpha << ::is_same_v<int, int> << "\n";     // Output: true
  std::cout << std::boolalpha << ::is_same_v<int, double> << "\n";  // Output: false

  // X1<int, 10>::X2<int, double>{}.f();

  std::cout << Y<double, float> << "\n";  // Output: "?"
  std::cout << Y<int, float> << "\n";     // Output: "int-specialized"

  std::cout << x<int> << "\n";   // Output: "?"
  std::cout << x<int*> << "\n";  // Output: "pointer"
}