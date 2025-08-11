#include <chrono>
#include <cstddef>
#include <mutex>
#include <print>
#include <thread>

void func1() {
  std::size_t shared_data = 100;
  std::mutex mtx;

  std::jthread thr1{[&](std::stop_token stoken) -> void {
    while (!stoken.stop_requested()) {
      mtx.lock();
      ++shared_data;
      std::print("func1: shared_data = {}\n", shared_data);
      mtx.unlock();
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
  }};

  std::jthread thr2{[&](std::stop_token stoken) -> void {
    while (!stoken.stop_requested()) {
      mtx.lock();
      --shared_data;
      std::print("func2: shared_data = {}\n", shared_data);
      mtx.unlock();
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
  }};

  std::this_thread::sleep_for(std::chrono::seconds(5));
}

int main() {
  try {
    func1();
  } catch (const std::exception &e) {
    std::print("Exception: {}\n", e.what());
  } catch (...) {
    std::print("Unknown exception occurred\n");
  }
}