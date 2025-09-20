#include <cstddef>
#include <memory>
#include <mutex>
#include <print>
#include <thread>

template <typename T, size_t Capacity> class LockQueue {
public:
  LockQueue() : _max_size(Capacity + 1), _data(_alloc.allocate(_max_size)) {}

  ~LockQueue() {
    std::lock_guard<std::mutex> lock{_mutex};
    // 调用析构函数
    while (_size-- > 0) {
      std::destroy_at(_data + _head);
      _head = (_head + 1) % _max_size;
    }
    // 回收内存
    _alloc.deallocate(_data, _max_size);
  }

  template <typename... Args> bool emplace(Args &&...args) {
    std::lock_guard<std::mutex> lock{_mutex};
    if (_size == _max_size - 1) {
      return false;
    }
    std::construct_at(_data + _tail, std::forward<Args>(args)...);
    _tail = (_tail + 1) % _max_size;
    ++_size;
    return true;
  }

  bool pop(T &value) {
    std::lock_guard<std::mutex> lock{_mutex};
    if (_size == 0) {
      return false;
    }
    value = std::move(*(_data + _head));
    _head = (_head + 1) % _max_size;
    --_size;
    return true;
  }

private:
  size_t _max_size;
  size_t _head{0};
  size_t _tail{0};
  size_t _size{0};

  std::allocator<T> _alloc;
  T *_data;
  std::mutex _mutex;
};

int main() {
  LockQueue<std::string, 10> queue;

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