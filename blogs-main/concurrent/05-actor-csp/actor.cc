#include <mutex>
#include <print>
#include <queue>
#include <functional>
#include <condition_variable>
#include <thread>

class Actor {
  using Message = std::function<void()>;

private:
  void run() {
    while (true) {
      Message msg;
      {
        std::unique_lock<std::mutex> lock{_mtx};
        _cv.wait(lock, [this]() -> bool {
          return _stop || !_msgBox.empty();
        });

        if (_stop && _msgBox.empty()) {
          break;
        }
        msg = std::move(_msgBox.front());
        _msgBox.pop();
      }
      msg();
    }
  }

public:
  Actor() : _actorThread(&Actor::run, this) {}

  virtual ~Actor() {
    {
      std::lock_guard<std::mutex> lock{_mtx};
      _stop = true;
    }
    _cv.notify_one();
  }

  void send(Message msg) {
    {
      std::lock_guard<std::mutex> lock{_mtx};
      if (_stop) {
        throw std::runtime_error("Actor has been stopped, cannot send message.");
      }
      _msgBox.push(std::move(msg));
    }
    _cv.notify_one();
  }

private:
  std::mutex _mtx;
  std::jthread _actorThread;
  std::condition_variable _cv;

  bool _stop{false};
  std::queue<Message> _msgBox;
};

class CounterActor final : public Actor {
public:
  void increment() {
    this->send([this] -> void {
      ++_count;
      std::print("count: {}\n", _count);
    });
  }

private:
  int _count{0};
};

int main() {
  CounterActor counter_actor;

  // 主线程进行投递
  for (int i = 0; i < 10; ++i) {
    counter_actor.increment();
  }

  std::this_thread::sleep_for(std::chrono::seconds(3));

  // 其他线程进行投递
  std::jthread thr{[&counter_actor]() {
    for (int i = 0; i < 10; ++i) {
      counter_actor.increment();
    }
  }};

  std::this_thread::sleep_for(std::chrono::seconds(3));
}