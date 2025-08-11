#include <mutex>
#include <print>
#include <thread>

void func1() {
  int shared_data = 100;
  std::mutex mtx;

  std::jthread thr{[&](std::stop_token stoken) -> void {
    while (!stoken.stop_requested()) {
      {
        std::lock_guard<std::mutex> lock(mtx);
        ++shared_data;
        std::print("func1: shared_data = {}\n", shared_data);
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
  }};

  std::this_thread::sleep_for(std::chrono::seconds(5));
}

void func2() {
  int shared_data = 100;
  std::mutex mtx1;
  std::mutex mtx2;

  std::jthread thr{[&]() -> void {
    std::unique_lock<std::mutex> lock1(mtx1, std::defer_lock);
    std::unique_lock<std::mutex> lock2(mtx2, std::defer_lock);
    std::lock(lock1, lock2); // 同时锁定多个互斥量，避免死锁
    ++shared_data;
    std::print("func2: shared_data = {}\n", shared_data);
  }};

  std::this_thread::sleep_for(std::chrono::seconds(2));
}

void func3() {
  int shared_data = 100;
  std::mutex mtx1;
  std::mutex mtx2;

  std::jthread thr{[&]() -> void {
    std::scoped_lock<std::mutex, std::mutex> lock(mtx1, mtx2);
    ++shared_data;
    std::print("func3: shared_data = {}\n", shared_data);
  }};

  std::this_thread::sleep_for(std::chrono::seconds(2));
}

int main() {
  try {
    func3();
  } catch (const std::exception &e) {
    std::print("Exception: {}\n", e.what());
  } catch (...) {
    std::print("Unknown exception occurred\n");
  }
  return 0;
}