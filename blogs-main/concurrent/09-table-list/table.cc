#include <algorithm>
#include <chrono>
#include <cstddef>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <utility>
#include <vector>

template <typename Key, typename Value, typename Hash = std::hash<Key>>
class LockLookupTable
{
private:
  class alignas(std::hardware_destructive_interference_size) bucket_type
  {
  private:
    using bucket_value = std::pair<Key, Value>;
    using bucket_data = std::vector<bucket_value>;
    using bucket_iterator = typename bucket_data::iterator;

    bucket_iterator find_entry_for(const Key& key)
    {
      return std::find_if(_data.begin(), _data.end(),
                          [&](const bucket_value& item) -> bool { return item.first == key; });
    }

  public:
    void add_or_update_node(const Key& key, const Value& value)
    {
      std::unique_lock<std::shared_mutex> lock{_sh_mtx};
      if (auto found = find_entry_for(key); found != _data.end())
        found->second = value;
      else
        _data.emplace_back(key, value);
    }

    void delete_node(const Key& key)
    {
      std::unique_lock<std::shared_mutex> lock{_sh_mtx};
      if (auto found = find_entry_for(key); found != _data.end())
      {
        *found = std::move(_data.back());
        _data.pop_back();
      }
    }

    Value value_for(const Key& key, const Value& default_value)
    {
      std::shared_lock<std::shared_mutex> lock{_sh_mtx};
      auto found = find_entry_for(key);
      return (found == _data.end()) ? default_value : found->second;
    }

  private:
    bucket_data _data;
    mutable std::shared_mutex _sh_mtx;
  };

public:
  LockLookupTable(unsigned size = 19, const Hash& hasher = Hash()) : _buckets(size), _hasher(hasher)
  {
    for (auto& bucket : _buckets)
    {
      bucket = std::make_unique<bucket_type>();
    }
  }

  LockLookupTable(const LockLookupTable&) = delete;
  LockLookupTable& operator=(const LockLookupTable&) = delete;

  void add_or_update_table(const Key& key, const Value& value = Value())
  {
    get_bucket(key).add_or_update_node(key, value);
  }

  void delete_table(const Key& key)
  {
    get_bucket(key).delete_node(key);
  }

  Value value_for(const Key& key, const Value& default_value = Value())
  {
    return get_bucket(key).value_for(key, default_value);
  }

private:
  std::vector<std::unique_ptr<bucket_type>> _buckets;
  Hash _hasher;

  bucket_type& get_bucket(const Key& key) const
  {
    const size_t index = _hasher(key) % _buckets.size();
    return *_buckets[index];
  }
};

class ConcurrencyPerformanceTester
{
private:
  static constexpr size_t DEFAULT_OPERATIONS = 1000000;
  static constexpr size_t DEFAULT_TABLE_SIZE = 997;
  static const int MAX_THREADS;

public:
  template <typename Key, typename Value>
  static void test_read_performance()
  {
    std::cout << "\n=== 并发读取性能测试 ===\n";

    LockLookupTable<Key, Value> table(DEFAULT_TABLE_SIZE);

    for (size_t i = 0; i < DEFAULT_OPERATIONS; ++i)
    {
      table.add_or_update_table(static_cast<Key>(i), static_cast<Value>(i * 2));
    }

    for (int thread_count = 1; thread_count <= MAX_THREADS; thread_count *= 2)
    {
      test_concurrent_reads(table, thread_count);
    }
  }

  template <typename Key, typename Value>
  static void test_mixed_operations_performance()
  {
    std::cout << "\n=== 混合读写操作性能测试 ===\n";

    LockLookupTable<Key, Value> table(DEFAULT_TABLE_SIZE);

    for (size_t i = 0; i < DEFAULT_OPERATIONS / 10; ++i)
    {
      table.add_or_update_table(static_cast<Key>(i), static_cast<Value>(i * 2));
    }

    for (int thread_count = 1; thread_count <= MAX_THREADS; thread_count *= 2)
    {
      test_mixed_operations(table, thread_count);
    }
  }

  template <typename Key, typename Value>
  static void test_contention_performance()
  {
    std::cout << "\n=== 高争用情况性能测试 ===\n";

    LockLookupTable<Key, Value> table(DEFAULT_TABLE_SIZE);
    const size_t hotspot_keys = 100;

    for (size_t i = 0; i < hotspot_keys; ++i)
    {
      table.add_or_update_table(static_cast<Key>(i), static_cast<Value>(i * 2));
    }

    for (int thread_count = 1; thread_count <= MAX_THREADS; thread_count *= 2)
    {
      test_hotspot_contention(table, thread_count, hotspot_keys);
    }
  }

  template <typename Key, typename Value>
  static void run_all_tests()
  {
    std::cout << "开始并发性能测试...\n";
    std::cout << "硬件并发线程数: " << MAX_THREADS << "\n";
    std::cout << "测试数据量: " << DEFAULT_OPERATIONS << "\n";
    std::cout << "哈希表大小: " << DEFAULT_TABLE_SIZE << "\n";

    test_read_performance<Key, Value>();
    test_mixed_operations_performance<Key, Value>();
    test_contention_performance<Key, Value>();

    std::cout << "\n所有测试完成！\n";
  }

private:
  template <typename Key, typename Value>
  static void test_concurrent_reads(LockLookupTable<Key, Value>& table, int thread_count)
  {
    std::vector<std::thread> threads;
    std::atomic<long long> total_operations{0};

    auto start_time = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < thread_count; ++i)
    {
      threads.emplace_back(
          [&table, &total_operations, i, thread_count]()
          {
            long long local_ops = 0;
            const size_t operations_per_thread = DEFAULT_OPERATIONS / thread_count;
            const size_t start_key = i * operations_per_thread;

            for (size_t j = 0; j < operations_per_thread; ++j)
            {
              Key key = static_cast<Key>((start_key + j) % DEFAULT_OPERATIONS);
              volatile auto value = table.value_for(key, static_cast<Value>(-1));
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

    double ops_per_second = (double)total_operations * 1000.0 / duration.count();

    std::cout << "线程数: " << thread_count << ", 耗时: " << duration.count() << "ms"
              << ", 操作数: " << total_operations << ", 读取速度: " << static_cast<long long>(ops_per_second)
              << " ops/sec\n";
  }

  template <typename Key, typename Value>
  static void test_mixed_operations(LockLookupTable<Key, Value>& table, int thread_count)
  {
    std::vector<std::thread> threads;
    std::atomic<long long> total_reads{0};
    std::atomic<long long> total_writes{0};

    auto start_time = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < thread_count; ++i)
    {
      threads.emplace_back(
          [&table, &total_reads, &total_writes, i, thread_count]()
          {
            long long local_reads = 0;
            long long local_writes = 0;
            const size_t operations_per_thread = DEFAULT_OPERATIONS / thread_count / 10;

            for (size_t j = 0; j < operations_per_thread; ++j)
            {
              Key key = static_cast<Key>((i * operations_per_thread + j) % (DEFAULT_OPERATIONS / 10));

              if (j % 5 == 0)
              {
                table.add_or_update_table(key, static_cast<Value>(j));
                ++local_writes;
              }
              else
              {
                volatile auto value = table.value_for(key, static_cast<Value>(-1));
                ++local_reads;
              }
            }

            total_reads += local_reads;
            total_writes += local_writes;
          });
    }

    for (auto& t : threads)
    {
      t.join();
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    long long total_ops = total_reads + total_writes;
    double ops_per_second = (double)total_ops * 1000.0 / duration.count();

    std::cout << "线程数: " << thread_count << ", 耗时: " << duration.count() << "ms"
              << ", 读取: " << total_reads << ", 写入: " << total_writes
              << ", 混合速度: " << static_cast<long long>(ops_per_second) << " ops/sec\n";
  }

  template <typename Key, typename Value>
  static void test_hotspot_contention(LockLookupTable<Key, Value>& table, int thread_count, size_t hotspot_keys)
  {
    std::vector<std::thread> threads;
    std::atomic<long long> total_operations{0};

    auto start_time = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < thread_count; ++i)
    {
      threads.emplace_back(
          [&table, &total_operations, hotspot_keys, thread_count]()
          {
            long long local_ops = 0;
            const size_t operations_per_thread = DEFAULT_OPERATIONS / thread_count / 100;

            for (size_t j = 0; j < operations_per_thread; ++j)
            {
              Key key = static_cast<Key>(j % hotspot_keys);

              if (j % 10 == 0)
              {
                table.add_or_update_table(key, static_cast<Value>(j));
              }
              else
              {
                volatile auto value = table.value_for(key, static_cast<Value>(-1));
              }
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
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

    double ops_per_second = (double)total_operations * 1000000.0 / std::max((long long)duration.count(), 1LL);

    std::cout << "线程数: " << thread_count << ", 耗时: " << duration.count() << "μs"
              << ", 操作数: " << total_operations << ", 争用速度: " << static_cast<long long>(ops_per_second)
              << " ops/sec"
              << " (热点键数: " << hotspot_keys << ")\n";
  }
};

const int ConcurrencyPerformanceTester::MAX_THREADS = std::thread::hardware_concurrency();

int main()
{
  ConcurrencyPerformanceTester::run_all_tests<int, int>();
  return 0;
}