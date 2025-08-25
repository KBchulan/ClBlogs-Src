#include <atomic>
#include <cstddef>
#include <memory>
#include <print>
#include <utility>
#include <thread>


template <typename T, size_t Capacity> class SuperQueue {
private:
  struct alignas(64) Slot {
    std::atomic<size_t> sequence{0};
    T data;
  };

  static constexpr size_t cache_line_size = 64;

  // 确保容量是2的幂，便于位运算优化
  static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be power of 2");

  alignas(cache_line_size) std::atomic<size_t> enqueue_pos{0};
  alignas(cache_line_size) std::atomic<size_t> dequeue_pos{0};

  Slot buffer[Capacity];

public:
  SuperQueue() {
    // 初始化每个slot的序列号
    for (size_t i = 0; i < Capacity; ++i) {
      buffer[i].sequence.store(i, std::memory_order_relaxed);
    }
  }

  ~SuperQueue() {
    // 析构剩余的元素
    size_t front = dequeue_pos.load(std::memory_order_relaxed);
    size_t back = enqueue_pos.load(std::memory_order_relaxed);

    while (front != back) {
      size_t pos = front & (Capacity - 1); // 位运算取模
      if (buffer[pos].sequence.load(std::memory_order_relaxed) == front + 1) {
        std::destroy_at(&buffer[pos].data);
      }
      ++front;
    }
  }

  template <typename... Args> bool emplace(Args &&...args) {
    Slot *slot;
    size_t pos = enqueue_pos.load(std::memory_order_relaxed);

    while (true) {
      slot = &buffer[pos & (Capacity - 1)];
      size_t seq = slot->sequence.load(std::memory_order_acquire);
      intptr_t diff = (intptr_t)seq - (intptr_t)pos;

      if (diff == 0) {
        // 该位置可以插入，尝试占据这个位置
        if (enqueue_pos.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed)) {
          break;
        }
      } else if (diff < 0) {
        // 队列满
        return false;
      } else {
        // 其他生产者已经占用了这个位置
        pos = enqueue_pos.load(std::memory_order_relaxed);
      }
    }

    // 在占据的槽中构造元素
    std::construct_at(&slot->data, std::forward<Args>(args)...);

    // 更新序列号，使数据对消费者可见
    slot->sequence.store(pos + 1, std::memory_order_release);

    return true;
  }

  bool pop(T &result) {
    Slot *slot;
    size_t pos = dequeue_pos.load(std::memory_order_relaxed);

    while (true) {
      slot = &buffer[pos & (Capacity - 1)];
      size_t seq = slot->sequence.load(std::memory_order_acquire);
      intptr_t diff = (intptr_t)seq - (intptr_t)(pos + 1);

      if (diff == 0) {
        // 尝试更新出队位置
        if (dequeue_pos.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed)) {
          break;
        }
      } else if (diff < 0) {
        // 队列空
        return false;
      } else {
        // 其他消费者已经占用了这个位置
        pos = dequeue_pos.load(std::memory_order_relaxed);
      }
    }

    // 读取数据
    result = std::move(slot->data);
    std::destroy_at(&slot->data);

    // 更新序列号，使位置对生产者可用
    slot->sequence.store(pos + Capacity, std::memory_order_release);

    return true;
  }
};

int main() {
  SuperQueue<std::string, 16> queue;

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