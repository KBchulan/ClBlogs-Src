#include <iostream>

template <typename... Args>
void printr(const Args&... args)
{
  ((std::cout << args << ' '), ...);
  std::cout << '\n';
}

template <typename... Args>
void printl(const Args&... args)
{
  (..., (std::cout << args << ' '));
  std::cout << '\n';
}

template <int... Args>
constexpr int v_right = (Args - ...);

template <int... Args>
constexpr int v_left = (... - Args);

template <typename... Args>
void printwt(const Args&... args)
{
  (std::cout << ... << args);
  std::cout << '\n';
}

int main()
{
  std::cout << v_right<1, 2, 3> << '\n';
  std::cout << v_left<1, 2, 3> << '\n';

  ::printl("lusa", 1, 2.5);
}