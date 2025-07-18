#pragma once

#include <memory>

class Widget {
public:
  Widget();
  ~Widget();

  // 由于三五法则，这些需要实现
  Widget(const Widget& other);
  Widget& operator=(const Widget& other);
  Widget(Widget&& other) noexcept;
  Widget& operator=(Widget&& other) noexcept;

  void doSomething();

private:
  class Impl;
  std::unique_ptr<Impl> pimpl;
};