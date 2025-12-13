#include <cstddef>
#include <iostream>
// #include <mutex>

template <typename T>
struct Test
{
  Test(T _t) : t(_t)
  {
  }
  T t;
};

Test(int) -> Test<std::size_t>;

template <typename T, std::size_t size>
struct Array
{
  T arr[size];
};

template <typename T, typename... Args>
Array(T, Args...) -> Array<T, 1 + sizeof...(Args)>;

template <typename T = int>
struct X
{
};

struct Test2
{
  X<> x;
  static inline X<> x3;
};

template <typename T>
struct Y
{
  void f(T)
  {
  }

  template <typename ...Args>
  void g(Args... args){}
};

int main()
{
  X x1;
  X<> x2;

  ::Array arr{1, 2, 3, 4, 5};
  for (int i = 0; i < 5; ++i)
  {
    std::cout << arr.arr[i] << " ";
  }
  std::cout << "\n";

  Test<int> t1{1};
  Test t3{2};

  // std::mutex mtx;
  // std::lock_guard lock1{mtx};
  // std::lock_guard<std::mutex> lock2{mtx};
}