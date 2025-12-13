#include <ios>
template <typename T1, typename T2>
auto add(T1 a, T2 b)
{
  return a + b;
}

template <>
auto add(double a, int b)
{
  return a + b + 0.5;  // Specialized behavior for double
}

template <typename T>
struct is_void
{
  static constexpr bool value = false;
};

template <>
struct is_void<void>
{
  static constexpr bool value = true;
};

template <typename T>
constexpr bool is_void_v = is_void<T>::value;

template <typename T>
constexpr const char* s = "??";

template <>
constexpr const char* s<void> = "void";

template <typename T>
void f() {}

void f2() {f<int>();}

// template <>
// void f<int>() {}

template <typename T>
struct X;

template <>
struct X<int>;

#include <iostream>

int main()
{
  std::cout << s<int> << '\n';
  std::cout << s<void> << '\n';

  std::cout << std::boolalpha << is_void_v<int> << '\n';
  std::cout << std::boolalpha << is_void_v<void> << '\n';

  std::cout << std::boolalpha << is_void<int>::value << '\n';
  std::cout << std::boolalpha << is_void<void>::value << '\n';

  std::cout << add(3, 4) << "\n";      // Uses primary template
  std::cout << add(2.5, 3) << "\n";    // Uses specialized template
  std::cout << add(1.5, 2.5) << "\n";  // Uses primary template
  return 0;
}