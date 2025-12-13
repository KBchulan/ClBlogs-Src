// template <typename T>
// void f()
// {
//   T::type* p;
// }

#include <vector>
template <typename T>
const typename T::type& f2(const T&)
{
  return 0;
}

template <typename T>
void foo(std::vector<T>& vec)
{
  typename std::vector<T>::const_iterator it = vec.begin();
}

template <typename T>
void foo(const std::vector<T>& v)
{
  using iter_t = typename std::vector<T>::const_iterator;
  iter_t it = v.begin();
}

// template <typename T>
// struct S
// {
//   template <typename U>
//   void foo()
//   {
//   }
// };

// template <typename T>
// void bar()
// {
//   S<T> s;
//   // s.foo<T>();           // 错误！编译器把 < 当成小于号
//   s.template foo<T>();  // 正确！告诉编译器 foo 是模板
// }

#include <iostream>

void g(double)
{
  std::cout << "g(double)\n";
}

template <class T>
struct S
{
  void f() const
  {
    g(1);  // g 不依赖 T，是非待决名，在这里就绑定到 g(double)
  }
};

void g(int)
{
  std::cout << "g(int)\n";
}  // 定义太晚了！

int main()
{
  g(1);  // 调用 g(int)
  S<int> s;
  s.f();  // 调用 g(double)，不是 g(int)！
}
