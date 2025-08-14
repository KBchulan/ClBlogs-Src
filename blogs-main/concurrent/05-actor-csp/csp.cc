#include <condition_variable>
#include <mutex>
#include <optional>
#include <print>
#include <thread>

template <typename T> class Channel {
public:
  void send(T data) {
    std::unique_lock<std::mutex> lock{_mtx};
    if (_stop) {
      throw std::runtime_error("Channel has been stopped, cannot send data.");
    }

    _send_cv.wait(lock, [this]() { return !_data.has_value(); });
    _data = std::move(data);
    _recv_cv.notify_one();
  }

  std::optional<T> recv() {
    std::unique_lock<std::mutex> lock{_mtx};

    _recv_cv.wait(lock, [this]() { return _stop || _data.has_value(); });

    if (_stop && !_data.has_value()) {
      return std::nullopt;
    }

    std::optional<T> data = std::move(_data);
    _data.reset();
    _send_cv.notify_one();
    return data;
  }

  void stop() {
    {
      std::lock_guard<std::mutex> lock{_mtx};
      _stop = true;
    }
    _send_cv.notify_all();
    _recv_cv.notify_all();
  }

private:
  bool _stop{false};
  std::optional<T> _data;

  std::mutex _mtx;
  std::condition_variable _send_cv;
  std::condition_variable _recv_cv;
};

void test_ping_pong();

void test_many();

int main() {
  test_many();
}

void test_ping_pong() {
  Channel<std::string> ping_chan;
  Channel<std::string> pong_chan;

  std::jthread thr1{[&] -> void {
    ping_chan.send("ping");
    auto msg = pong_chan.recv();
    if (msg.has_value()) {
      std::print("thr1 recv: {}\n", *msg);
    }
  }};

  std::jthread thr2{[&] -> void {
    auto msg = ping_chan.recv();
    if (msg.has_value()) {
      std::print("thr2 recv: {}\n", *msg);
      pong_chan.send("pong");
    }
  }};

  thr1.join();
  thr2.join();
}

void test_many() {
  Channel<std::string> chan;

  std::jthread sender{[&] -> void {
    for (int i = 0; i < 50; ++i) {
      chan.send("message " + std::to_string(i));
    }
    chan.stop();
  }};

  std::jthread receiver{[&] -> void {
    while (true) {
      auto msg = chan.recv();
      if (!msg.has_value()) {
        break;
      }
      std::print("recv: {}\n", *msg);
    }
  }};
}