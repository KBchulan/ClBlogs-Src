#include <chrono>
#include <condition_variable>
#include <mutex>
#include <optional>
#include <print>
#include <queue>
#include <thread>

template <typename T> class BufferedChannel {
public:
  explicit BufferedChannel(size_t capacity) : _capacity(capacity) {
    if (capacity == 0) {
      throw std::invalid_argument(
          "BufferedChannel capacity must be at least 1.");
    }
  }

  void send(T value) {
    std::unique_lock<std::mutex> lock(_mtx);

    _cv_not_full.wait(lock, [this]() -> bool {
      return _buffer.size() < _capacity || _closed;
    });

    if (_closed) {
      throw std::runtime_error("Send on a closed channel");
    }

    _buffer.push(std::move(value));

    _cv_not_empty.notify_one();
  }

  std::optional<T> receive() {
    std::unique_lock<std::mutex> lock(_mtx);

    _cv_not_empty.wait(lock, [this]() { return !_buffer.empty() || _closed; });

    if (_buffer.empty() && _closed) {
      return std::nullopt;
    }

    T value = std::move(_buffer.front());
    _buffer.pop();

    _cv_not_full.notify_one();

    return value;
  }

  void close() {
    {
      std::lock_guard<std::mutex> lock(_mtx);
      _closed = true;
    }
    _cv_not_full.notify_all();
    _cv_not_empty.notify_all();
  }

private:
  std::mutex _mtx;
  std::condition_variable _cv_not_full;
  std::condition_variable _cv_not_empty;
  std::queue<T> _buffer;
  const size_t _capacity;
  bool _closed{false};
};

int main() {
  BufferedChannel<int> ch(5);

  std::jthread producer([&ch]() {
    for (int i = 0; i < 10; ++i) {
      std::print("Producer: sending {}\n", i);
      ch.send(i);
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    std::print("Producer: finished sending, closing channel.\n");
    ch.close();
  });

  std::jthread consumer([&ch]() {
    while (true) {
      auto val = ch.receive();
      if (val.has_value()) {
        std::print("Consumer: received {}\n", *val);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
      } else {
        std::print("Consumer: channel closed and empty, exiting.\n");
        break;
      }
    }
  });


}