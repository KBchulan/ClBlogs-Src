#include <atomic>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <new>
#include <optional>
#include <thread>
#include <utility>
#include <vector>

#if defined(__cpp_lib_hardware_interference_size)
constexpr size_t CACHE_LINE_SIZE = std::hardware_destructive_interference_size;
#else
constexpr size_t CACHE_LINE_SIZE = 64;
#endif

class BackOff
{
public:
  void pause()
  {
    for (unsigned i = 0; i < _count; i++) cpu_pause();
    if (_count < 1024) _count *= 2;
  }

  void reset()
  {
    _count = 1;
  }

private:
  void cpu_pause()
  {
// x86 架构
#if defined(__x86_64__) || defined(__i386__)
    __builtin_ia32_pause();
// ARM 架构
#elif defined(__arm__) || defined(__aarch64__)
    __asm__ __volatile__("yield");
// 其他架构
#else
    std::this_thread::yield();
#endif
  }

  int _count = 1;
};

template <typename T>
struct Node
{
  Node<T>* _next;
  alignas(T) std::array<std::byte, sizeof(T)> _storage;

  T* data() noexcept
  {
    return std::launder(reinterpret_cast<T*>(_storage.data()));
  }
};

template <typename T>
struct TaggedNode
{
  Node<T>* _ptr = nullptr;
  uintptr_t _tag = 0;

  bool operator==(const TaggedNode& other) const
  {
    return this->_ptr == other._ptr && this->_tag == other._tag;
  }
};

template <typename T, size_t Capacity>
class NodePool
{
public:
  NodePool()
  {
    _nodes_storage.reserve(Capacity);
    for (unsigned i = 0; i < Capacity; i++) _nodes_storage.emplace_back();
    for (unsigned i = 0; i < Capacity - 1; i++) _nodes_storage[i]._next = &_nodes_storage[i + 1];
    _nodes_storage[Capacity - 1]._next = nullptr;
    _head.store({&_nodes_storage[0], 0}, std::memory_order_relaxed);
  }

  Node<T>* acquire()
  {
    BackOff backoff;
    TaggedNode<T> old_head = _head.load(std::memory_order_relaxed);
    while (old_head._ptr)
    {
      TaggedNode<T> new_head;
      new_head._ptr = old_head._ptr->_next;
      new_head._tag = old_head._tag + 1;

      if (_head.compare_exchange_weak(old_head, new_head, std::memory_order_acquire, std::memory_order_relaxed))
      {
        return old_head._ptr;
      }
      backoff.pause();
    }
    return nullptr;
  }

  void release(Node<T>* node)
  {
    BackOff backoff;
    TaggedNode<T> old_head = _head.load(std::memory_order_relaxed);
    TaggedNode<T> new_head;
    do
    {
      node->_next = old_head._ptr;
      new_head._ptr = node;
      new_head._tag = old_head._tag + 1;

      if (_head.compare_exchange_weak(old_head, new_head, std::memory_order_release, std::memory_order_relaxed))
      {
        break;
      }
      backoff.pause();
    } while (true);
  }

private:
  alignas(CACHE_LINE_SIZE) std::atomic<TaggedNode<T>> _head{};
  std::vector<Node<T>> _nodes_storage;
};

template <typename T, size_t Capacity>
class TreiberStack
{
  static_assert(std::is_nothrow_constructible_v<T>, "T must be nothrow constructible for lock-free safety");
  static_assert(std::is_nothrow_move_constructible_v<T>, "T must be nothrow move constructible for lock-free safety");

public:
  TreiberStack() = default;
  ~TreiberStack()
  {
    while (pop().has_value())
    {
    }
  }

  TreiberStack(const TreiberStack&) = delete;
  TreiberStack& operator=(const TreiberStack&) = delete;

  template <typename... Args>
  bool emplace(Args&&... args)
  {
    Node<T>* node = _pool.acquire();
    if (!node)
    {
      return false;
    }

    std::construct_at(node->data(), std::forward<Args>(args)...);

    BackOff backoff;
    TaggedNode<T> old_head = _head.load(std::memory_order_relaxed);
    TaggedNode<T> new_head;
    do
    {
      node->_next = old_head._ptr;
      new_head._ptr = node;
      new_head._tag = old_head._tag + 1;
      if (_head.compare_exchange_weak(old_head, new_head, std::memory_order_release, std::memory_order_relaxed))
      {
        return true;
      }
      backoff.pause();
    } while (true);
  }

  bool push(const T& value)
  {
    return emplace(value);
  }

  bool push(T&& value)
  {
    return emplace(std::move(value));
  }

  std::optional<T> pop()
  {
    BackOff backoff;
    TaggedNode<T> old_head = _head.load(std::memory_order_relaxed);
    TaggedNode<T> new_head;
    while (old_head._ptr)
    {
      Node<T>* next = old_head._ptr->_next;
      new_head._ptr = next;
      new_head._tag = old_head._tag + 1;

      if (_head.compare_exchange_weak(old_head, new_head, std::memory_order_acquire, std::memory_order_relaxed))
      {
        T value = std::move(*old_head._ptr->data());
        std::destroy_at(old_head._ptr->data());
        _pool.release(old_head._ptr);
        return value;
      }
      backoff.pause();
    }
    return std::nullopt;
  }

private:
  alignas(CACHE_LINE_SIZE) std::atomic<TaggedNode<T>> _head{};
  alignas(CACHE_LINE_SIZE) NodePool<T, Capacity> _pool;
};

// =================================================================
// 吞吐量测试函数
// =================================================================
#include <iostream>
int main()
{
  constexpr size_t STACK_CAPACITY = 16384;
  constexpr int NUM_THREADS = 8;
  constexpr int OPS_PER_THREAD = 5000000;

  TreiberStack<int, STACK_CAPACITY> stack;

  for (int i = 0; i < NUM_THREADS * 100; ++i) stack.push(i);

  std::cout << "Starting throughput test with " << NUM_THREADS << " threads, " << OPS_PER_THREAD
            << " operations per thread." << std::endl;

  std::vector<std::thread> threads;
  threads.reserve(NUM_THREADS);

  auto start_time = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < NUM_THREADS; ++i)
  {
    threads.emplace_back(
        [&stack]()
        {
          for (int j = 0; j < OPS_PER_THREAD; ++j)
          {
            if (j % 2 == 0)
              stack.push(j);
            else
              stack.pop();
          }
        });
  }

  for (auto& t : threads) t.join();

  auto end_time = std::chrono::high_resolution_clock::now();

  std::chrono::duration<double, std::milli> duration_ms = end_time - start_time;
  double total_ops = static_cast<double>(NUM_THREADS) * OPS_PER_THREAD;
  double throughput = total_ops / (duration_ms.count() / 1000.0);

  std::cout << "Test finished." << std::endl;
  std::cout << "Total time: " << duration_ms.count() << " ms" << std::endl;
  std::cout << "Total operations: " << std::fixed << total_ops << std::endl;
  std::cout << "Throughput: " << std::fixed << throughput / 1e6 << " M ops/sec" << std::endl;

  return 0;
}
