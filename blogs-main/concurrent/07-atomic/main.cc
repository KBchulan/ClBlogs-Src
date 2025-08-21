#include <atomic>
#include <cassert>
#include <cstdint>
#include <exception>
#include <print>
#include <thread>

void func1() {
  std::atomic_bool flag1{false};
  std::atomic_bool flag2{false};
  std::atomic_int32_t counter{0};

  std::jthread thr1{[&] -> void {
    flag1.store(true, std::memory_order_relaxed);
    flag2.store(true, std::memory_order_relaxed);
  }};

  std::jthread thr2{[&] -> void {
    while (!flag2.load(std::memory_order_relaxed)) {
      std::this_thread::yield();
    }

    if (flag1.load(std::memory_order_relaxed)) {
      counter.fetch_add(1, std::memory_order_relaxed);
    }
  }};

  thr1.join();
  thr2.join();
  assert(counter.load(std::memory_order_relaxed) == 1);
}

void func2() {
  std::atomic_bool flag1{false};
  std::atomic_bool flag2{false};
  std::atomic_int32_t counter{0};

  std::jthread thr1{[&] -> void {
    flag1.store(true, std::memory_order_relaxed);
    flag2.store(true, std::memory_order_release);
  }};

  std::jthread thr2{[&] -> void {
    while (!flag2.load(std::memory_order_acquire)) {
      std::this_thread::yield();
    }

    if (flag1.load(std::memory_order_relaxed)) {
      counter.fetch_add(1, std::memory_order_relaxed);
    }
  }};

  thr1.join();
  thr2.join();
  assert(counter.load(std::memory_order_relaxed) == 1);
}

void func3() {
  std::atomic_int x{0};
  int expected_value = 1;
  x.compare_exchange_strong(expected_value, 10, std::memory_order_acq_rel, std::memory_order_relaxed);
  x.fetch_add(1, std::memory_order_acq_rel);
}

void func4() {
  std::atomic_bool flag1{false};
  std::atomic_bool flag2{false};

  int32_t z = 0;

  std::jthread thr1{[&] -> void {
    flag1.store(true, std::memory_order_release);
  }};

  std::jthread thr2{[&] -> void {
    flag2.store(true, std::memory_order_release);
  }};

  std::jthread thr3{[&] -> void {
    while (!flag1.load(std::memory_order_acquire)) {}
    if (flag2.load(std::memory_order_acquire)) {
      z = 1;
    }
  }};

  std::jthread thr4{[&] -> void {
    while (!flag2.load(std::memory_order_acquire)) {}
    if (flag1.load(std::memory_order_acquire)) {
      z = 2;
    }
  }};

  thr1.join();
  thr2.join();
  thr3.join();
  thr4.join();
  std::print("z = {}\n", z);
}

void func5() {
  std::atomic_bool flag1{false};
  std::atomic_bool flag2{false};

  int32_t z = 0;

  std::jthread thr1{[&] -> void {
    flag1.store(true, std::memory_order_seq_cst);
  }};

  std::jthread thr2{[&] -> void {
    flag2.store(true, std::memory_order_seq_cst);
  }};

  std::jthread thr3{[&] -> void {
    while (!flag1.load(std::memory_order_seq_cst)) {}
    if (flag2.load(std::memory_order_seq_cst)) {
      z = 1;
    }
  }};

  std::jthread thr4{[&] -> void {
    while (!flag2.load(std::memory_order_seq_cst)) {}
    if (flag1.load(std::memory_order_seq_cst)) {
      z = 2;
    }
  }};

  thr1.join();
  thr2.join();
  thr3.join();
  thr4.join();
  std::print("z = {}\n", z);
}

int main() {
  try {
    func4();
  } catch (std::exception& e) {
    std::print("Exception: {}\n", e.what());
  } catch (...) {
    std::print("Unknown exception occurred\n");
  }
}