#include <mutex>
#include <print>
#include <stop_token>
#include <thread>

void func() {
  std::timed_mutex mtx;
  std::size_t shared_data = 100;

  std::jthread thr1{[&](std::stop_token token) -> void {
    while (!token.stop_requested()) {
      if (mtx.try_lock_for(std::chrono::milliseconds(100))) {
        ++shared_data;
        std::print("func1: shared_data = {}\n", shared_data);
        mtx.unlock();
      } else {
        std::print("func1: could not acquire lock, retrying...\n");
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
  }};

  std::jthread thr2{[&](std::stop_token token) -> void {
    while (!token.stop_requested()) {
      if (mtx.try_lock_for(std::chrono::milliseconds(100))) {
        --shared_data;
        std::print("func2: shared_data = {}\n", shared_data);
        mtx.unlock();
      } else {
        std::print("func2: could not acquire lock, retrying...\n");
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
  }};

  std::this_thread::sleep_for(std::chrono::seconds(5));
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