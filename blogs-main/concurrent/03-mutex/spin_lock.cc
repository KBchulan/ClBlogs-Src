#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <print>
#include <thread>

class SpinLock {
public:
  void lock() {
    // test_and_set返回之前的值
    // 如果之前是false，设置为true并返回false，获取锁成功
    // 如果之前是true，保持为true并返回true，继续自旋
    while (_flag.test_and_set(std::memory_order_acquire)) {}
  }

  void unlock() { _flag.clear(std::memory_order_release); }

private:
  std::atomic_flag _flag = ATOMIC_FLAG_INIT;
};

class AdaptiveBackoffSpinLock {
public:
  void lock() {
    if (try_lock()) {
      return; // 成功获取锁
    }

    auto start = std::chrono::steady_clock::now();
    lock_adaptive(start);
  }

  void unlock() {
    auto hold_time = std::chrono::steady_clock::now() - _lock_acquire_time;

    update_statistics(hold_time);
    _flag.store(false, std::memory_order_release);
  }

  bool try_lock() {
    if (_flag.load(std::memory_order_relaxed)) {
      return false; // 锁已被占用
    }

    bool expected = false;
    bool success = _flag.compare_exchange_strong(expected, true, std::memory_order_acquire, std::memory_order_relaxed);

    if (success) {
      _lock_acquire_time = std::chrono::steady_clock::now();
    }
    return success;
  }

private:
  std::atomic_bool _flag{false};
  std::atomic_uint32_t _avg_hold_time_us{10};  // 平均持有时间 us
  std::chrono::steady_clock::time_point _lock_acquire_time; // 锁获取时间

  inline void cpu_pause() {
    // x86 架构
    #if defined (__x86_64__) || defined (__i386__)
      __builtin_ia32_pause();
    // ARM 架构
    #elif defined (__arm__) || defined (__aarch64__)
      __asm__ __volatile__("yield");
    // 其他架构
    #else
      std::this_thread::yield();
    # endif
  }

  void lock_adaptive(std::chrono::steady_clock::time_point start) {
    uint32_t expected_wait_us = _avg_hold_time_us.load(std::memory_order_relaxed);

    if (expected_wait_us < 5) {
      // 短时间，纯自旋
      spin_only();
    } else if (expected_wait_us < 50) {
      // 中等时间，自旋+退避
      spin_with_backoff();
    } else {
      // 长时间，自旋+让步
      hybrid_wait(expected_wait_us);
    }
  }

   void update_statistics(std::chrono::steady_clock::duration hold_time) {
        auto hold_us = std::chrono::duration_cast<std::chrono::microseconds>(hold_time).count();

        // 简单的指数移动平均
        uint32_t old_avg = _avg_hold_time_us.load(std::memory_order_relaxed);
        uint32_t new_avg = (old_avg * 7 + hold_us) / 8;
        _avg_hold_time_us.store(new_avg, std::memory_order_relaxed);
    }


  void spin_only() {
    while (true) {
      while (_flag.load(std::memory_order_relaxed)) {
        cpu_pause();
      }

      if (try_lock()) {
        return; // 成功获取锁
      }
    }
  }

  void spin_with_backoff() {
    int backoff = 1;

    while (true) {
      int wait_cycles = 0;
      while (_flag.load(std::memory_order_relaxed)) {
        for (int i = 0; i < backoff; ++i) {
          cpu_pause();
        }
        if (++wait_cycles > 1000) {
          std::this_thread::yield();
          wait_cycles = 0;
        }
      }

      if (try_lock()) {
        return;
      }

      backoff = std::min(backoff * 2, 1024); // 指数退避，最大值限制
    }
  }

  void hybrid_wait(uint32_t expected_wait_us) {
    // 自旋一会
    auto spin_until = std::chrono::steady_clock::now() + std::chrono::microseconds(expected_wait_us / 4);

    while (std::chrono::steady_clock::now() < spin_until) {
      if (!_flag.load(std::memory_order_relaxed)) {
        if (try_lock()) {
          return; // 成功获取锁
        }
      }
      cpu_pause();
    }

    while (true) {
      std::this_thread::yield();

      for (int i = 0; i < 10; ++i) {
        if (!_flag.load(std::memory_order_relaxed)) {
          if (try_lock()) {
            return; // 成功获取锁
          }
        }
        cpu_pause();
      }
    }
  }
};

void func() {
  size_t shared_data = 100;
  AdaptiveBackoffSpinLock spin_lock;

  std::jthread thr1{[&](std::stop_token stoken) -> void {
    while (!stoken.stop_requested()) {
      spin_lock.lock();
      ++shared_data;
      std::print("func1: shared_data = {}\n", shared_data);
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
      spin_lock.unlock();
    }
  }};

  std::jthread thr2{[&](std::stop_token stoken) -> void {
    while (!stoken.stop_requested()) {
      spin_lock.lock();
      --shared_data;
      std::print("func2: shared_data = {}\n", shared_data);
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
      spin_lock.unlock();
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