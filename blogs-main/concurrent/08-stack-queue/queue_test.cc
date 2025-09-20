#include "queue.cc"

#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

void test_basic_operations()
{
  std::cout << "开始基本操作测试..." << std::endl;

  // 测试 LockQueue
  LockQueue<int> queue1;

  // 测试基本的推入和弹出
  queue1.push(1);
  queue1.push(2);
  queue1.push(3);

  assert(queue1.size() == 3);
  assert(!queue1.empty());

  // 测试 try_pop
  int value;
  assert(queue1.try_pop(value));
  assert(value == 1);

  assert(queue1.try_pop(value));
  assert(value == 2);

  assert(queue1.size() == 1);

  // 测试 emplace
  queue1.emplace(4);
  assert(queue1.size() == 2);

  // 测试清空
  queue1.clear();
  assert(queue1.empty());
  assert(queue1.size() == 0);

  // 测试 LockQueueInList
  LockQueueInList<int> queue2;

  queue2.push(10);
  queue2.push(20);

  assert(!queue2.empty());

  auto result = queue2.try_pop();
  assert(result.has_value());
  assert(result.value() == 10);

  result = queue2.try_pop();
  assert(result.has_value());
  assert(result.value() == 20);

  assert(queue2.empty());

  result = queue2.try_pop();
  assert(!result.has_value());

  std::cout << "基本操作测试通过！" << std::endl;
}

void test_concurrent_single_lock()
{
  std::cout << "开始单锁队列并发测试..." << std::endl;

  LockQueue<int> queue;
  const int producer_count = 4;
  const int consumer_count = 2;
  const int items_per_producer = 1000;

  std::atomic<int> produced{0};
  std::atomic<int> consumed{0};
  std::vector<std::thread> producers;
  std::vector<std::thread> consumers;

  auto start_time = std::chrono::steady_clock::now();

  // 创建生产者线程
  for (int i = 0; i < producer_count; ++i)
  {
    producers.emplace_back(
        [&queue, &produced, items_per_producer, i]()
        {
          for (int j = 0; j < items_per_producer; ++j)
          {
            queue.push(i * items_per_producer + j);
            produced.fetch_add(1);
          }
        });
  }

  // 创建消费者线程
  for (int i = 0; i < consumer_count; ++i)
  {
    consumers.emplace_back(
        [&queue, &consumed, producer_count, items_per_producer]()
        {
          int target = producer_count * items_per_producer;
          while (consumed.load() < target)
          {
            int value;
            if (queue.try_pop(value))
            {
              consumed.fetch_add(1);
            }
            else
            {
              std::this_thread::yield();
            }
          }
        });
  }

  // 等待所有线程完成
  for (auto& t : producers)
  {
    t.join();
  }
  for (auto& t : consumers)
  {
    t.join();
  }

  auto end_time = std::chrono::steady_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

  assert(produced.load() == producer_count * items_per_producer);
  assert(consumed.load() == producer_count * items_per_producer);
  assert(queue.empty());

  std::cout << "单锁队列并发测试通过！" << std::endl;
  std::cout << "处理了 " << produced.load() << " 个元素，耗时: " << duration.count() << "ms" << std::endl;
}

void test_concurrent_double_lock()
{
  std::cout << "开始双锁队列并发测试..." << std::endl;

  LockQueueInList<int> queue;
  const int producer_count = 4;
  const int consumer_count = 2;
  const int items_per_producer = 1000;

  std::atomic<int> produced{0};
  std::atomic<int> consumed{0};
  std::vector<std::thread> producers;
  std::vector<std::thread> consumers;

  auto start_time = std::chrono::steady_clock::now();

  // 创建生产者线程
  for (int i = 0; i < producer_count; ++i)
  {
    producers.emplace_back(
        [&queue, &produced, items_per_producer, i]()
        {
          for (int j = 0; j < items_per_producer; ++j)
          {
            queue.push(i * items_per_producer + j);
            produced.fetch_add(1);
          }
        });
  }

  // 创建消费者线程
  for (int i = 0; i < consumer_count; ++i)
  {
    consumers.emplace_back(
        [&queue, &consumed, producer_count, items_per_producer]()
        {
          int target = producer_count * items_per_producer;
          while (consumed.load() < target)
          {
            auto result = queue.try_pop();
            if (result.has_value())
            {
              consumed.fetch_add(1);
            }
            else
            {
              std::this_thread::yield();
            }
          }
        });
  }

  // 等待所有线程完成
  for (auto& t : producers)
  {
    t.join();
  }
  for (auto& t : consumers)
  {
    t.join();
  }

  auto end_time = std::chrono::steady_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

  assert(produced.load() == producer_count * items_per_producer);
  assert(consumed.load() == producer_count * items_per_producer);
  assert(queue.empty());

  std::cout << "双锁队列并发测试通过！" << std::endl;
  std::cout << "处理了 " << produced.load() << " 个元素，耗时: " << duration.count() << "ms" << std::endl;
}

void test_blocking_operations()
{
  std::cout << "开始阻塞操作测试..." << std::endl;

  LockQueue<int> queue1;
  LockQueueInList<int> queue2;

  // 测试 LockQueue 的阻塞弹出
  std::thread producer1(
      [&queue1]()
      {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        queue1.push(42);
      });

  auto start = std::chrono::steady_clock::now();
  auto result1 = queue1.pop();
  auto end = std::chrono::steady_clock::now();
  auto duration1 = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

  assert(result1.has_value());
  assert(result1.value() == 42);
  assert(duration1.count() >= 100);  // 应该至少等待了 100ms

  producer1.join();

  // 测试 LockQueueInList 的阻塞弹出
  std::thread producer2(
      [&queue2]()
      {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        queue2.push(99);
      });

  start = std::chrono::steady_clock::now();
  auto result2 = queue2.pop();
  end = std::chrono::steady_clock::now();
  auto duration2 = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

  assert(result2.has_value());
  assert(result2.value() == 99);
  assert(duration2.count() >= 100);  // 应该至少等待了 100ms

  producer2.join();

  std::cout << "阻塞操作测试通过！" << std::endl;
}

void large_scale_stress_test()
{
  std::cout << "开始大规模压力测试..." << std::endl;

  const int producer_count = 8;
  const int consumer_count = 4;
  const int items_per_producer = 100000;  // 10倍数据量

  std::cout << "测试配置: " << producer_count << "个生产者, " << consumer_count << "个消费者, 每个生产者产生"
            << items_per_producer << "个元素" << std::endl;

  // 测试单锁队列性能
  {
    std::cout << "\n[大规模测试] 测试单锁队列..." << std::endl;
    LockQueue<int> queue;
    std::atomic<int> produced{0};
    std::atomic<int> consumed{0};

    auto start_time = std::chrono::steady_clock::now();

    std::vector<std::thread> threads;

    // 生产者
    for (int i = 0; i < producer_count; ++i)
    {
      threads.emplace_back(
          [&queue, &produced, items_per_producer, i]()
          {
            for (int j = 0; j < items_per_producer; ++j)
            {
              queue.push(i * items_per_producer + j);
              produced.fetch_add(1);
            }
          });
    }

    // 消费者
    for (int i = 0; i < consumer_count; ++i)
    {
      threads.emplace_back(
          [&queue, &consumed, producer_count, items_per_producer]()
          {
            int target = producer_count * items_per_producer;
            while (consumed.load() < target)
            {
              int value;
              if (queue.try_pop(value))
              {
                consumed.fetch_add(1);
              }
              else
              {
                std::this_thread::yield();
              }
            }
          });
    }

    for (auto& t : threads)
    {
      t.join();
    }

    auto end_time = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    int total_items = producer_count * items_per_producer;
    double throughput = (double)total_items * 1000.0 / duration.count();

    std::cout << "单锁队列: " << duration.count() << "ms, 吞吐量: " << (int)throughput << " 元素/秒" << std::endl;
  }

  // 测试双锁队列性能
  {
    std::cout << "[大规模测试] 测试双锁队列..." << std::endl;
    LockQueueInList<int> queue;
    std::atomic<int> produced{0};
    std::atomic<int> consumed{0};

    auto start_time = std::chrono::steady_clock::now();

    std::vector<std::thread> threads;

    // 生产者
    for (int i = 0; i < producer_count; ++i)
    {
      threads.emplace_back(
          [&queue, &produced, items_per_producer, i]()
          {
            for (int j = 0; j < items_per_producer; ++j)
            {
              queue.push(i * items_per_producer + j);
              produced.fetch_add(1);
            }
          });
    }

    // 消费者
    for (int i = 0; i < consumer_count; ++i)
    {
      threads.emplace_back(
          [&queue, &consumed, producer_count, items_per_producer]()
          {
            int target = producer_count * items_per_producer;
            while (consumed.load() < target)
            {
              auto result = queue.try_pop();
              if (result.has_value())
              {
                consumed.fetch_add(1);
              }
              else
              {
                std::this_thread::yield();
              }
            }
          });
    }

    for (auto& t : threads)
    {
      t.join();
    }

    auto end_time = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    int total_items = producer_count * items_per_producer;
    double throughput = (double)total_items * 1000.0 / duration.count();

    std::cout << "双锁队列: " << duration.count() << "ms, 吞吐量: " << (int)throughput << " 元素/秒" << std::endl;
  }
}

void extreme_concurrency_test()
{
  std::cout << "\n开始极限并发测试..." << std::endl;

  const int producer_count = 16;
  const int consumer_count = 8;
  const int items_per_producer = 5000000;

  std::cout << "极限配置: " << producer_count << "个生产者, " << consumer_count << "个消费者, 每个生产者产生"
            << items_per_producer << "个元素" << std::endl;

  // 测试单锁队列
  {
    std::cout << "\n[极限测试] 测试单锁队列..." << std::endl;
    LockQueue<int> queue;
    std::atomic<int> produced{0};
    std::atomic<int> consumed{0};

    auto start_time = std::chrono::steady_clock::now();

    std::vector<std::thread> threads;

    for (int i = 0; i < producer_count; ++i)
    {
      threads.emplace_back(
          [&queue, &produced, items_per_producer, i]()
          {
            for (int j = 0; j < items_per_producer; ++j)
            {
              queue.push(i * items_per_producer + j);
              produced.fetch_add(1);
            }
          });
    }

    for (int i = 0; i < consumer_count; ++i)
    {
      threads.emplace_back(
          [&queue, &consumed, producer_count, items_per_producer]()
          {
            int target = producer_count * items_per_producer;
            while (consumed.load() < target)
            {
              int value;
              if (queue.try_pop(value))
              {
                consumed.fetch_add(1);
              }
              else
              {
                std::this_thread::yield();
              }
            }
          });
    }

    for (auto& t : threads)
    {
      t.join();
    }

    auto end_time = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    int total_items = producer_count * items_per_producer;
    double throughput = (double)total_items * 1000.0 / duration.count();

    std::cout << "单锁队列: " << duration.count() << "ms, 吞吐量: " << (int)throughput << " 元素/秒" << std::endl;
  }

  // 测试双锁队列
  {
    std::cout << "[极限测试] 测试双锁队列..." << std::endl;
    LockQueueInList<int> queue;
    std::atomic<int> produced{0};
    std::atomic<int> consumed{0};

    auto start_time = std::chrono::steady_clock::now();

    std::vector<std::thread> threads;

    for (int i = 0; i < producer_count; ++i)
    {
      threads.emplace_back(
          [&queue, &produced, items_per_producer, i]()
          {
            for (int j = 0; j < items_per_producer; ++j)
            {
              queue.push(i * items_per_producer + j);
              produced.fetch_add(1);
            }
          });
    }

    for (int i = 0; i < consumer_count; ++i)
    {
      threads.emplace_back(
          [&queue, &consumed, producer_count, items_per_producer]()
          {
            int target = producer_count * items_per_producer;
            while (consumed.load() < target)
            {
              auto result = queue.try_pop();
              if (result.has_value())
              {
                consumed.fetch_add(1);
              }
              else
              {
                std::this_thread::yield();
              }
            }
          });
    }

    for (auto& t : threads)
    {
      t.join();
    }

    auto end_time = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    int total_items = producer_count * items_per_producer;
    double throughput = (double)total_items * 1000.0 / duration.count();

    std::cout << "双锁队列: " << duration.count() << "ms, 吞吐量: " << (int)throughput << " 元素/秒" << std::endl;
  }
}

void performance_comparison()
{
  std::cout << "\n开始基础性能对比测试..." << std::endl;

  const int producer_count = 2;
  const int consumer_count = 2;
  const int items_per_producer = 10000;

  // 测试单锁队列性能
  {
    LockQueue<int> queue;
    std::atomic<int> produced{0};
    std::atomic<int> consumed{0};

    auto start_time = std::chrono::steady_clock::now();

    std::vector<std::thread> threads;

    // 生产者
    for (int i = 0; i < producer_count; ++i)
    {
      threads.emplace_back(
          [&queue, &produced, items_per_producer, i]()
          {
            for (int j = 0; j < items_per_producer; ++j)
            {
              queue.push(i * items_per_producer + j);
              produced.fetch_add(1);
            }
          });
    }

    // 消费者
    for (int i = 0; i < consumer_count; ++i)
    {
      threads.emplace_back(
          [&queue, &consumed, producer_count, items_per_producer]()
          {
            int target = producer_count * items_per_producer;
            while (consumed.load() < target)
            {
              int value;
              if (queue.try_pop(value))
              {
                consumed.fetch_add(1);
              }
              else
              {
                std::this_thread::yield();
              }
            }
          });
    }

    for (auto& t : threads)
    {
      t.join();
    }

    auto end_time = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

    std::cout << "单锁队列: " << duration.count() << " 微秒" << std::endl;
  }

  // 测试双锁队列性能
  {
    LockQueueInList<int> queue;
    std::atomic<int> produced{0};
    std::atomic<int> consumed{0};

    auto start_time = std::chrono::steady_clock::now();

    std::vector<std::thread> threads;

    // 生产者
    for (int i = 0; i < producer_count; ++i)
    {
      threads.emplace_back(
          [&queue, &produced, items_per_producer, i]()
          {
            for (int j = 0; j < items_per_producer; ++j)
            {
              queue.push(i * items_per_producer + j);
              produced.fetch_add(1);
            }
          });
    }

    // 消费者
    for (int i = 0; i < consumer_count; ++i)
    {
      threads.emplace_back(
          [&queue, &consumed, producer_count, items_per_producer]()
          {
            int target = producer_count * items_per_producer;
            while (consumed.load() < target)
            {
              auto result = queue.try_pop();
              if (result.has_value())
              {
                consumed.fetch_add(1);
              }
              else
              {
                std::this_thread::yield();
              }
            }
          });
    }

    for (auto& t : threads)
    {
      t.join();
    }

    auto end_time = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

    std::cout << "双锁队列: " << duration.count() << " 微秒" << std::endl;
  }
}

int main()
{
  std::cout << "=== 队列测试开始 ===" << std::endl;

  try
  {
    // 基础功能测试
    test_basic_operations();
    test_blocking_operations();

    // 中等规模并发测试
    test_concurrent_single_lock();
    test_concurrent_double_lock();
    performance_comparison();

    // 大规模压力测试
    large_scale_stress_test();

    // 极限并发测试
    extreme_concurrency_test();

    std::cout << "\n=== 所有测试通过！ ===" << std::endl;
  }
  catch (const std::exception& e)
  {
    std::cerr << "测试失败: " << e.what() << std::endl;
    return 1;
  }
  catch (...)
  {
    std::cerr << "未知错误导致测试失败" << std::endl;
    return 1;
  }

  return 0;
}