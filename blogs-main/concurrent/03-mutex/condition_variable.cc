#include <condition_variable>
#include <mutex>
#include <print>
#include <queue>
#include <thread>

void func() {
  std::condition_variable produce_cv;
  std::condition_variable consume_cv;
  std::mutex mtx;
  std::queue<int> data_queue;
  auto push = [&](int value) -> void {
    std::unique_lock<std::mutex> lock(mtx);

    consume_cv.wait(lock, [&]() { return data_queue.size() < 10; });

    data_queue.push(value);
    std::print("Produced: {}, queue size: {}\n", value, data_queue.size());
    produce_cv.notify_one();
  };

  auto pop = [&]() -> void {
    std::unique_lock<std::mutex> lock(mtx);

    produce_cv.wait(lock, [&]() { return !data_queue.empty(); });

    int value = data_queue.front();
    data_queue.pop();
    std::print("Consumed: {}, queue size: {}\n", value, data_queue.size());
    consume_cv.notify_one();
  };

  std::jthread thr1{[&]() -> void {
    for (int i = 0; i < 20; ++i) {
      push(i);
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  }};

  std::jthread thr2{[&]() -> void {
    for (int i = 0; i < 20; ++i) {
      pop();
      std::this_thread::sleep_for(std::chrono::milliseconds(150));
    }
  }};

  std::this_thread::sleep_for(std::chrono::seconds(10));
}

int main() {
  try {
    func();
  } catch (const std::exception &e) {
    std::print("Exception: {}\n", e.what());
  } catch (...) {
    std::print("Unknown exception occurred\n");
  }
}