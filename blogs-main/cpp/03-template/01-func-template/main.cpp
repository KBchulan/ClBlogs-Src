#include <cstddef>
#include <iostream>
#include <iterator>
#include <numeric>
#include <print>
#include <type_traits>
#include <utility>

#include "test.h"

struct Test
{
  int _val{};

  Test(int val) : _val(val)
  {
  }

  bool operator>(const Test& other) const
  {
    return _val > other._val;
  }
};

template <typename T>
T max(T a, T b)
{
  return a > b ? a : b;
}

template <typename T>
T min(const T& a, const T& b)
{
  return a > b ? a : b;
}

template <typename T>
void foo(T&& t)
{
}

template <typename... Args>
void wrapper(Args&&... args)
{
  wrapper(std::forward<Args>(args)...);
}

template <typename T = int>
void f()
{
}

template <typename T1, typename T2, typename RT = decltype(true ? T1{} : T2{})>
RT max1(const T1& a, const T2& b)
{
  return a > b ? a : b;
}

template <typename T1, typename T2, typename RT = decltype(true ? std::declval<T1>() : std::declval<T2>())>
RT max2(const T1& a, const T2& b)
{
  return a > b ? a : b;
}

template <typename T1, typename T2>
auto max3_1(const T1& a, const T2& b) -> decltype(true ? a : b)
{
  return a > b ? a : b;
}

template <typename T1, typename T2>
std::common_type_t<T1, T2> max3_2(const T1& a, const T2& b)
{
  return a > b ? a : b;
}

auto max4(const auto& a, const auto& b)
{
  return a > b ? a : b;
}

using T1 = decltype(true ? 1 : 2.2);
using T2 = decltype(false ? 1 : 2.2);

template <std::size_t N = 10>
void func1()
{
  std::print("N is: {}\n", N);
}

template <typename T>
void func2(T)
{
  std::print("template\n");
}

void func2(int)
{
  std::print("int\n");
}

template <typename... Args>
void print(const Args&... args)
{
  int _[]{(std::cout << args << ' ', 0)...};
}

template <typename T, std::size_t N, typename... Args>
void f(const T (&array)[N], Args... index)
{
  print(array[index]...);
}

template <typename... Args, typename RT = std::common_type<Args...>>
RT sum(Args... args)
{
  RT _[] = {static_cast<RT>(args)...};
  return std::accumulate(std::begin(_), std::end(_), RT{});
}

void func3()
{
  int arr[] = {
#include "array.txt"
  };
  f(arr, 0, 2, 4);
}

int main()
{
  ttt();

  func3();

  int array[] = {1, 2, 3};
  f(array, 0, 1);

  func2(2);
  func2(1.2);

  std::cout << max(10, 20) << '\n';
  std::cout << max(Test(10), Test(15))._val << '\n';
  std::cout << max<double>(1.3, 2.4) << '\n';

  std::cout << min(1, 2) << '\n';

  max(static_cast<double>(1), 2.5);

  using namespace std::string_literals;
  ::max("hello"s, std::string("sss"));

  foo(1);

  f<double>();

  int x = 1;
  int y = 2;
  const int& a = x;
  const int& b = y;

  auto v1 = max1(1, 1.5);
  auto v2 = max2(1, 1.5);
  auto v31 = max3_1(a, b);
  auto v32 = max3_2(1, 1.5);
  auto v4 = max4(1, 1.5);

  std::cout << std::is_same_v<decltype(v31), const int&> << '\n';

  func1();
}