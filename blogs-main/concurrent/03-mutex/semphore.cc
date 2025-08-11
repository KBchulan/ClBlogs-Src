#include <print>
#include <semaphore>
#include <thread>

void func() {
  std::binary_semaphore mts(0);

  std::jthread thr{[&]() -> void {
    mts.acquire(); // 等待来自主线程的信号量
    std::print("Thread has acquired the semaphore, proceeding...\n");
  }};

  std::this_thread::sleep_for(std::chrono::seconds(2));
  std::print("Main thread is releasing the semaphore...\n");
  mts.release(); // 释放信号量，允许线程继续执行
  std::this_thread::sleep_for(std::chrono::seconds(2));
}

int main() {
  try {
    func();
  } catch (const std::exception &e) {
    std::print("Exception: {}\n", e.what());
  } catch (...) {
    std::print("Unknown exception occurred\n");
  }
  return 0;
}