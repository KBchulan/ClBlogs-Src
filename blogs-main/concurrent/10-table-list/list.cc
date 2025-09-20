#include <atomic>
#include <chrono>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <thread>
#include <utility>
#include <vector>

template <typename T>
class LockList
{
private:
  struct alignas(std::hardware_destructive_interference_size) Node
  {
    std::shared_mutex _mtx;
    std::optional<T> _data;
    std::unique_ptr<Node> _next;

    Node() = default;

    Node(const T& data) : _data(std::make_optional<T>(data))
    {
    }
  };

public:
  LockList() = default;
  ~LockList()
  {
    remove_if([](auto&) -> bool { return true; });
  }

  LockList(const LockList&) = delete;
  LockList& operator=(const LockList&) = delete;

  void push_front(const T& value)
  {
    std::unique_ptr<Node> new_node = std::make_unique<Node>(value);
    std::unique_lock<std::shared_mutex> lock{_head._mtx};
    new_node->_next = std::move(_head._next);
    _head._next = std::move(new_node);
  }

  template <typename Predicate>
  void remove_if(Predicate p)
  {
    Node* current = &_head;
    std::unique_lock<std::shared_mutex> lock_cur{current->_mtx};

    while (Node* next = current->_next.get())
    {
      std::unique_lock<std::shared_mutex> lock_next{next->_mtx};
      if (p(next->_data.value()))
      {
        auto old_next = std::move(current->_next);
        current->_next = std::move(old_next->_next);
        lock_next.unlock();
      }
      else
      {
        lock_cur.unlock();
        current = next;
        lock_cur = std::move(lock_next);
      }
    }
  }

  template <typename Predicate>
  std::optional<T> find_first_of(Predicate p)
  {
    Node* current = &_head;
    std::shared_lock<std::shared_mutex> cur_lock{current->_mtx};
    while (Node* next = current->_next.get())
    {
      std::shared_lock<std::shared_mutex> next_lock{next->_mtx};
      cur_lock.unlock();
      if (p(next->_data.value()))
      {
        T result = next->_data.value();
        next_lock.unlock();
        return result;
      }
      current = next;
      cur_lock = std::move(next_lock);
    }
    return std::nullopt;
  }

  template <typename Function>
  void for_each(Function func)
  {
    Node* current = &_head;
    std::shared_lock<std::shared_mutex> cur_lock{current->_mtx};
    while (Node* next = current->_next.get())
    {
      std::shared_lock<std::shared_mutex> next_lock{next->_mtx};
      cur_lock.unlock();
      func(next->_data.value());
      current = next;
      cur_lock = std::move(next_lock);
    }
  }

private:
  Node _head;  // 虚节点
};

class ConcurrencyListTester
{
private:
  static constexpr size_t DEFAULT_OPERATIONS = 5000;
  static const int MAX_THREADS;

public:
  template <typename T>
  static void test_concurrent_push_performance()
  {
    std::cout << "\n=== 并发插入性能测试 ===\n";

    for (int thread_count = 1; thread_count <= MAX_THREADS; thread_count *= 2)
    {
      test_concurrent_push<T>(thread_count);
    }
  }

  template <typename T>
  static void test_concurrent_find_performance()
  {
    std::cout << "\n=== 并发查找性能测试 ===\n";

    LockList<T> list;
    for (size_t i = 0; i < DEFAULT_OPERATIONS; ++i)
    {
      list.push_front(static_cast<T>(i));
    }

    for (int thread_count = 1; thread_count <= MAX_THREADS; thread_count *= 2)
    {
      test_concurrent_find(list, thread_count);
    }
  }

  template <typename T>
  static void test_mixed_operations_performance()
  {
    std::cout << "\n=== 混合操作性能测试 ===\n";

    for (int thread_count = 1; thread_count <= MAX_THREADS; thread_count *= 2)
    {
      test_mixed_operations<T>(thread_count);
    }
  }

  template <typename T>
  static void run_all_tests()
  {
    std::cout << "开始有锁链表并发性能测试...\n";
    std::cout << "硬件并发线程数: " << MAX_THREADS << "\n";
    std::cout << "测试数据量: " << DEFAULT_OPERATIONS << "\n";

    test_concurrent_push_performance<T>();
    test_concurrent_find_performance<T>();
    test_mixed_operations_performance<T>();

    std::cout << "\n所有测试完成！\n";
  }

private:
  template <typename T>
  static void test_concurrent_push(int thread_count)
  {
    LockList<T> list;
    std::vector<std::thread> threads;
    std::atomic<long long> total_operations{0};

    auto start_time = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < thread_count; ++i)
    {
      threads.emplace_back(
          [&list, &total_operations, i, thread_count]()
          {
            long long local_ops = 0;
            const size_t operations_per_thread = DEFAULT_OPERATIONS / thread_count;
            const size_t start_value = i * operations_per_thread;

            for (size_t j = 0; j < operations_per_thread; ++j)
            {
              list.push_front(static_cast<T>(start_value + j));
              ++local_ops;
            }

            total_operations += local_ops;
          });
    }

    for (auto& t : threads)
    {
      t.join();
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    double ops_per_second = (double)total_operations * 1000.0 / std::max((long long)duration.count(), 1LL);

    std::cout << "线程数: " << thread_count << ", 耗时: " << duration.count() << "ms"
              << ", 操作数: " << total_operations << ", 插入速度: " << static_cast<long long>(ops_per_second)
              << " ops/sec\n";
  }

  template <typename T>
  static void test_concurrent_find(LockList<T>& list, int thread_count)
  {
    std::vector<std::thread> threads;
    std::atomic<long long> total_operations{0};
    std::atomic<long long> found_count{0};

    auto start_time = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < thread_count; ++i)
    {
      threads.emplace_back(
          [&list, &total_operations, &found_count, i, thread_count]()
          {
            long long local_ops = 0;
            long long local_found = 0;
            const size_t operations_per_thread = DEFAULT_OPERATIONS / thread_count;
            const size_t start_key = i * operations_per_thread;

            for (size_t j = 0; j < operations_per_thread; ++j)
            {
              T target = static_cast<T>((start_key + j) % DEFAULT_OPERATIONS);
              auto result = list.find_first_of([target](const T& x) { return x == target; });
              if (result)
              {
                ++local_found;
              }
              ++local_ops;
            }

            total_operations += local_ops;
            found_count += local_found;
          });
    }

    for (auto& t : threads)
    {
      t.join();
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    double ops_per_second = (double)total_operations * 1000.0 / std::max((long long)duration.count(), 1LL);

    std::cout << "线程数: " << thread_count << ", 耗时: " << duration.count() << "ms"
              << ", 操作数: " << total_operations << ", 找到: " << found_count
              << ", 查找速度: " << static_cast<long long>(ops_per_second) << " ops/sec\n";
  }

  template <typename T>
  static void test_mixed_operations(int thread_count)
  {
    LockList<T> list;
    std::vector<std::thread> threads;
    std::atomic<long long> total_push{0};
    std::atomic<long long> total_find{0};

    // 预填充一些数据
    for (size_t i = 0; i < DEFAULT_OPERATIONS / 10; ++i)
    {
      list.push_front(static_cast<T>(i));
    }

    auto start_time = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < thread_count; ++i)
    {
      threads.emplace_back(
          [&list, &total_push, &total_find, i, thread_count]()
          {
            long long local_push = 0;
            long long local_find = 0;
            const size_t operations_per_thread = DEFAULT_OPERATIONS / thread_count / 5;

            for (size_t j = 0; j < operations_per_thread; ++j)
            {
              T value = static_cast<T>(i * operations_per_thread + j);

              if (j % 3 == 0)
              {
                list.push_front(value);
                ++local_push;
              }
              else
              {
                auto result = list.find_first_of([value](const T& x) { return x == value; });
                ++local_find;
              }
            }

            total_push += local_push;
            total_find += local_find;
          });
    }

    for (auto& t : threads)
    {
      t.join();
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    long long total_ops = total_push + total_find;
    double ops_per_second = (double)total_ops * 1000.0 / std::max((long long)duration.count(), 1LL);

    std::cout << "线程数: " << thread_count << ", 耗时: " << duration.count() << "ms"
              << ", 插入: " << total_push << ", 查找: " << total_find
              << ", 混合速度: " << static_cast<long long>(ops_per_second) << " ops/sec\n";
  }
};

const int ConcurrencyListTester::MAX_THREADS = std::thread::hardware_concurrency();

int main()
{
  ConcurrencyListTester::run_all_tests<int>();
  return 0;
}
