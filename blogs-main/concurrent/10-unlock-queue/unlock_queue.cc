#include <atomic>
#include <cstddef>
#include <memory>
#include <print>
#include <thread>
#include <utility>


template <typename T, size_t Capacity> class UnlockQueue {
public:
  UnlockQueue() : _max_size(Capacity + 1), _data(_alloc.allocate(_max_size)) {}

  ~UnlockQueue() {
    size_t currentHead = _head.load(std::memory_order_acquire);
    size_t currentTail = _tail.load(std::memory_order_acquire);

    while (currentHead != currentTail) {
      std::destroy_at(_data + currentHead);
      currentHead = (currentHead + 1) % _max_size;
    }

    _alloc.deallocate(_data, _max_size);
  }

  template <typename... Args> bool emplace(Args &&...args) {
    while (true) {
      size_t currentTail = _tail.load(std::memory_order_relaxed);
      size_t nextTail = (currentTail + 1) % _max_size;

      if (nextTail == _head.load(std::memory_order_acquire)) {
        return false; // 队列满
      }

      if (_tail.compare_exchange_strong(currentTail, nextTail, std::memory_order_release, std::memory_order_relaxed)) {
        std::construct_at(_data + currentTail, std::forward<Args>(args)...);
        return true;
      }
    }
  }

  bool pop(T &value) {
    while (true) {
      size_t currentHead = _head.load(std::memory_order_relaxed);
      size_t nextHead = (currentHead + 1) % _max_size;

      if (currentHead == _tail.load(std::memory_order_acquire)) {
        return false;
      }

      if (_head.compare_exchange_strong(currentHead, nextHead, std::memory_order_release, std::memory_order_relaxed)) {
        value = std::move(*(_data + currentHead));
        std::destroy_at(_data + currentHead);
        return true;
      }
    }
  }

private:
  size_t _max_size;
  std::atomic<size_t> _head{0};
  std::atomic<size_t> _tail{0};

  std::allocator<T> _alloc;
  T *_data;
};

int main() {
  UnlockQueue<std::string, 10> queue;

  std::jthread thr1{[&queue] -> void {
    for (int i = 0; i < 20; ++i) {
      if (queue.emplace("Message " + std::to_string(i))) {
        std::print("Pushed: Message {}\n", i);
      } else {
        std::print("Queue full, failed to push: Message {}\n", i);
      }
    }
  }};

  std::jthread thr2{[&queue] -> void {
    size_t counter = 0;
    while (counter != 20) {
      std::string msg;
      if (queue.pop(msg)) {
        std::print("Popped: {}\n", msg);
        counter++;
      } else {
        std::print("Queue empty, failed to pop\n");
      }
    }
  }};

  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  return 0;
}