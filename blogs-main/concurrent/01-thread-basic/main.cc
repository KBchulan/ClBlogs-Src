#include <exception>
#include <functional>
#include <print>
#include <stop_token>
#include <string>
#include <thread>

void func1() {
  std::string str = "Hello, World!";

  std::thread thr([&str]() -> void {
    std::print("str is: {}", str);
  });

  thr.join();
}


class background_task {
public:
  void operator()() {
    std::print("Background task is running\n");
  }
};
void func2() {
  std::thread thr1((background_task()));
  thr1.join();

  std::thread thr2{background_task()};
  thr2.join();
}


void bac() {
  std::print("Background task is running\n");
}
void func3() {
  std::thread thr3(bac);
  thr3.join();
}


void func4() {
  std::thread thr4(std::bind(bac));
  thr4.join();
}


class Test {
public:
  void task() {
    std::print("Task is running\n");
  }
};
void func5() {
  Test test;
  std::thread thr5(&Test::task, &test);
  thr5.join();
}



class thread_guard {
private:
  std::thread& _t;

public:
  explicit thread_guard(std::thread& t) : _t(t) {}

  ~thread_guard() {
    if (_t.joinable()) {
      _t.join();
    }
  }

  thread_guard(const thread_guard&) = delete;
  thread_guard& operator=(const thread_guard&) = delete;
};
void func6() {
  std::thread thr{bac};
  thread_guard guard{thr};
}


void func7() {
  std::jthread thr{[](std::stop_token token) -> void {
    while (!token.stop_requested()) {
      std::print("Background task is running\n");
    }
    std::print("Background task is stopping\n");
  }};

  std::this_thread::sleep_for(std::chrono::seconds(1));
  thr.request_stop();
}


// void func8_err() {
//   std::string str = "Hello, World!";
//   std::jthread thr{[](std::string &str) -> void {
//     std::print("str is: {}\n", str);
//   }, str};
// }

void func8() {
  std::string str = "Hello, World!";
  std::jthread thr{[](std::string &str) -> void {
    std::print("str is: {}\n", str);
  }, std::ref(str)};
}

int main() {
  try {
    func8();
  } catch (const std::exception& e) {
    // 处理各个子线程正常退出
    std::print("Exception: {}\n", e.what());
  } catch (...) {
    // 处理各个子线程正常退出
    std::print("Unknown exception occurred\n");
  }
}