#include <atomic>
#include <condition_variable>
#include <future>
#include <mutex>
#include <print>
#include <queue>
#include <thread>
#include <type_traits>
#include <vector>

class ThreadPool {
  // 任务类型，把外界投递的任务都包装成此类型
  using Task = std::packaged_task<void()>;

private:
  explicit ThreadPool(unsigned int size = std::thread::hardware_concurrency()) {
    if (size < 1) {
      size = 2;
    }
    _poolSize.store(size, std::memory_order_relaxed);

    start();
  }

  ~ThreadPool() {
    _stop.store(true, std::memory_order_release);
    _cv.notify_all();
  }

  void start() {
    unsigned int size = _poolSize.load(std::memory_order_relaxed);

    _threads.reserve(size);
    for (unsigned int i = 0; i < size; i++) {
      _threads.emplace_back([this]() -> void {
        // 每个线程循环处理任务
        while (true) {
          Task task;
          {
            std::unique_lock<std::mutex> lock{_mtx};
            _cv.wait(lock, [this]() -> bool {
              return _stop.load(std::memory_order_acquire) || !_tasks.empty();
            });

            if (_stop.load(std::memory_order_acquire) && _tasks.empty()) {
              break;
            }

            task = std::move(_tasks.front());
            _tasks.pop();
          }
          task();
        }
      });
    }
  }

public:
  ThreadPool(const ThreadPool&) = delete;
  ThreadPool& operator=(const ThreadPool&) = delete;

  static ThreadPool& getInstance() {
    static ThreadPool instance;
    return instance;
  }

  template <typename Func, typename... Args>
  auto commit(Func&& func, Args&&... args) -> std::future<std::invoke_result_t<Func, Args...>> {
    using ReturnType = std::invoke_result_t<Func, Args...>;

    if (_stop.load(std::memory_order_relaxed)) {
      throw std::runtime_error("ThreadPool is stopped");
    }

    auto task = std::packaged_task<ReturnType()>(
      [func = std::forward<Func>(func), args...]() mutable -> ReturnType {
        return func(args...);
      });

    auto ret = task.get_future();
    {
      std::lock_guard<std::mutex> lock{_mtx};
      _tasks.emplace([task = std::move(task)]() mutable -> void {
        task();
      });
    }
    _cv.notify_one();
    return ret;
  }

private:
  std::mutex _mtx;
  std::condition_variable _cv;

  std::queue<Task> _tasks;
  std::vector<std::jthread> _threads;

  std::atomic_bool _stop{false};
  std::atomic_uint32_t _poolSize{0};
};

int main() {
  auto &pool = ThreadPool::getInstance();

  auto func = [](int x) -> void {
    std::print("Task executed with value: {}\n", x);
  };

  for (int i = 0; i < 10; ++i) {
    auto future = pool.commit(func, i);
    future.get();
  }
  std::print("All tasks completed.\n");
}