#include "widget.hpp"
#include <iostream>
#include <string>
#include <utility>

class Widget::Impl {
public:
  void doSomething() {
    std::cout << "Doing something in Impl with data: " << data << std::endl;
  }
  std::string data = "secret data";
};

Widget::Widget() : pimpl(std::make_unique<Impl>()) {}

Widget::~Widget() = default;

// --- 规则五的实现 ---
// 深拷贝构造函数
Widget::Widget(const Widget &other)
    : pimpl(std::make_unique<Impl>(*other.pimpl)) {}

// 拷贝赋值运算符
Widget &Widget::operator=(const Widget &other) {
  if (this != &other) {
    pimpl = std::make_unique<Impl>(*other.pimpl);
  }
  return *this;
}

// 移动构造函数
Widget::Widget(Widget &&other) noexcept = default;

// 移动赋值运算符
Widget &Widget::operator=(Widget &&other) noexcept = default;

void Widget::doSomething() { pimpl->doSomething(); }