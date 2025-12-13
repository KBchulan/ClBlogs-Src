#include <cstdio>
#include <type_traits>

template <typename A>
struct B
{
  using type = typename A::type;
};  // 实例化 B 会产生副作用

template <class T,
          class U = typename T::type,     // 如果 T 没有 type，这是 SFINAE 失败
          class V = typename B<T>::type>  // 如果 T 没有 type，这是硬错误（实例化 B 失败）
void foo(int)
{
  std::puts("SFINAE T::type B<T>::type");
}

template <typename T>
void foo(double)
{
  std::puts("SFINAE T");
}

template <typename T>
auto add(const auto& a, const auto& b) -> decltype(a + b)
{
  return a + b;
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
void f(T)
{
  std::puts("inter");
}

struct Test
{
  int value1;
  int value2;
};

int main()
{
  struct C
  {
    using type = int;
  };
  foo<B<C>>(1);  // 输出: SFINAE T::type B<T>::type
  foo<void>(1);  // 输出: SFINAE T（U 的代换先失败，是 SFINAE 错误）

  f(1);

  using aaa = decltype(&Test::value1);
}