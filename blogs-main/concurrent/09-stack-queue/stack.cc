#include <atomic>
#include <cassert>
#include <chrono>
#include <cstddef>
#include <iostream>
#include <mutex>
#include <optional>
#include <stack>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

template <typename T>
class LockStack
{
public:
  LockStack() = default;
  ~LockStack() = default;

  LockStack(const LockStack& other)
  {
    std::lock_guard<std::mutex> lock{other._mtx};
    _stack = other._stack;
  }

  LockStack& operator=(const LockStack& other)
  {
    if (this == &other) return *this;
    std::scoped_lock<std::mutex, std::mutex> lock{this->_mtx, other._mtx};
    this->_stack = other._stack;
    return *this;
  }

  LockStack(LockStack&& other) noexcept(std::is_nothrow_move_constructible_v<std::stack<T>>)
  {
    std::lock_guard<std::mutex> lock{other._mtx};
    _stack = std::move(other._stack);
  }

  LockStack& operator=(LockStack&& other) noexcept(std::is_nothrow_move_assignable_v<std::stack<T>>)
  {
    if (this == &other) return *this;
    std::scoped_lock<std::mutex, std::mutex> lock{this->_mtx, other._mtx};
    this->_stack = std::move(other._stack);
    return *this;
  }

  void push(const T& value)
  {
    std::lock_guard<std::mutex> lock{_mtx};
    _stack.push(value);
  }

  void push(T&& value)
  {
    std::lock_guard<std::mutex> lock{_mtx};
    _stack.push(std::move(value));
  }

  template <typename... Args>
  void emplace(Args&&... args)
  {
    std::lock_guard<std::mutex> lock{_mtx};
    _stack.emplace(std::forward<Args>(args)...);
  }

  bool try_pop(T& value)
  {
    std::lock_guard<std::mutex> lock{_mtx};
    if (_stack.empty()) return false;
    value = std::move_if_noexcept(_stack.top());
    _stack.pop();
    return true;
  }

  std::optional<T> pop() noexcept
  {
    static_assert(std::is_nothrow_move_constructible_v<T>, "this operation need T contains no throw in move cons");
    std::lock_guard<std::mutex> lock{_mtx};
    if (_stack.empty()) return std::nullopt;
    std::optional<T> result{std::move(_stack.top())};
    _stack.pop();
    return result;
  }

  std::optional<T> top() const
  {
    std::lock_guard<std::mutex> lock{_mtx};
    if (_stack.empty()) return std::nullopt;
    return std::optional<T>{_stack.top()};
  }

  bool empty() const noexcept
  {
    std::lock_guard<std::mutex> lock{_mtx};
    return _stack.empty();
  }

  size_t size() const noexcept
  {
    std::lock_guard<std::mutex> lock{_mtx};
    return _stack.size();
  }

  void clear() noexcept
  {
    std::lock_guard<std::mutex> lock{_mtx};
    while (!_stack.empty())
    {
      _stack.pop();
    }
  }

private:
  std::stack<T> _stack;
  mutable std::mutex _mtx;
};

void test_basic_operations()
{
  std::cout << "测试基本操作" << std::endl;

  LockStack<int> stack;

  // 测试空栈
  assert(stack.empty());
  assert(!stack.top().has_value());
  assert(!stack.pop().has_value());

  // 测试 push 和 top
  stack.push(10);
  assert(!stack.empty());
  assert(stack.top().value() == 10);

  stack.push(20);
  assert(stack.top().value() == 20);

  // 测试 pop
  auto val = stack.pop();
  assert(val.has_value());
  assert(val.value() == 20);
  assert(stack.top().value() == 10);

  val = stack.pop();
  assert(val.has_value());
  assert(val.value() == 10);
  assert(stack.empty());

  std::cout << "基本操作测试通过！" << std::endl;
}

void test_copy_constructor()
{
  std::cout << "测试拷贝构造函数" << std::endl;

  LockStack<int> stack1;
  stack1.push(1);
  stack1.push(2);
  stack1.push(3);

  LockStack<int> stack2(stack1);

  // 验证拷贝后的栈内容相同
  assert(stack2.top().value() == 3);
  stack2.pop();
  assert(stack2.top().value() == 2);
  stack2.pop();
  assert(stack2.top().value() == 1);

  // 验证原栈未受影响
  assert(stack1.top().value() == 3);

  std::cout << "拷贝构造函数测试通过！" << std::endl;
}

void test_concurrent_operations()
{
  std::cout << "测试并发操作" << std::endl;

  LockStack<int> stack;
  const int num_threads = 4;
  const int operations_per_thread = 1000;

  // 多线程 push
  std::vector<std::thread> push_threads;
  push_threads.reserve(num_threads);
  for (int i = 0; i < num_threads; ++i)
  {
    push_threads.emplace_back(
        [&stack, i, operations_per_thread]()
        {
          for (int j = 0; j < operations_per_thread; ++j)
          {
            stack.push(i * operations_per_thread + j);
          }
        });
  }

  for (auto& t : push_threads)
  {
    t.join();
  }

  int count = 0;
  while (!stack.empty())
  {
    auto val = stack.pop();
    if (val.has_value())
    {
      count++;
    }
  }

  assert(count == num_threads * operations_per_thread);
  std::cout << "成功插入和弹出 " << count << " 个元素" << std::endl;

  // 测试并发 push 和 pop
  std::atomic<int> push_count{0};
  std::atomic<int> pop_count{0};

  std::vector<std::thread> mixed_threads;

  // push 线程
  for (int i = 0; i < num_threads / 2; ++i)
  {
    mixed_threads.emplace_back(
        [&stack, &push_count, operations_per_thread]()
        {
          for (int j = 0; j < operations_per_thread; ++j)
          {
            stack.push(j);
            push_count++;
          }
        });
  }

  // pop 线程
  for (int i = 0; i < num_threads / 2; ++i)
  {
    mixed_threads.emplace_back(
        [&stack, &pop_count]()
        {
          while (pop_count < 1000)
          {  // 限制循环次数
            auto val = stack.pop();
            if (val.has_value())
            {
              pop_count++;
            }
            std::this_thread::sleep_for(std::chrono::microseconds(1));
          }
        });
  }

  for (auto& t : mixed_threads)
  {
    t.join();
  }

  std::cout << "Push 操作: " << push_count << ", Pop 操作: " << pop_count << std::endl;
  std::cout << "并发操作测试通过！" << std::endl;
}

int main()
{
  try
  {
    test_basic_operations();
    test_copy_constructor();
    test_concurrent_operations();
  }
  catch (const std::exception& e)
  {
    std::cerr << "测试失败: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
