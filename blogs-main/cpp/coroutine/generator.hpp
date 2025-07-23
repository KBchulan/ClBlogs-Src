#pragma once

#include <coroutine>
#include <optional>

template <typename T> class Generator {
public:
  // promise_type 是协程机制的钩子
  struct promise_type {
    std::optional<T> current_value;

    Generator<T> get_return_object() {
      return Generator{std::coroutine_handle<promise_type>::from_promise(*this)};
    }
    std::suspend_always initial_suspend() { return {}; }
    std::suspend_always final_suspend() noexcept { return {}; }
    void unhandled_exception() { std::terminate(); }
    void return_void() {}
    std::suspend_always yield_value(T value) {
      current_value = value;
      return {};
    }
  };

  // 让 Generator 可移动，但不可拷贝
  Generator(Generator &&other) noexcept : coro_handle_(std::__exchange(other.coro_handle_, {})) {}
  Generator &operator=(Generator &&other) noexcept {
    if (this != &other) {
      if (coro_handle_) {
        coro_handle_.destroy();
      }
      coro_handle_ = std::__exchange(other.coro_handle_, {});
    }
    return *this;
  }

  ~Generator() {
    if (coro_handle_) {
      coro_handle_.destroy();
    }
  }

  // 外部接口
  bool next() {
    if (coro_handle_ && !coro_handle_.done()) {
      coro_handle_.resume();
      return !coro_handle_.done();
    }
    return false;
  }

  T value() const { return *coro_handle_.promise().current_value; }

private:
  explicit Generator(std::coroutine_handle<promise_type> h) : coro_handle_(h) {}
  std::coroutine_handle<promise_type> coro_handle_;
};