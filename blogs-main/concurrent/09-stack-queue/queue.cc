#include <atomic>
#include <cassert>
#include <condition_variable>
#include <cstddef>
#include <mutex>
#include <optional>
#include <queue>
#include <type_traits>
#include <utility>

template <typename T>
class LockQueue
{
public:
  LockQueue() = default;
  ~LockQueue() = default;

  LockQueue(const LockQueue& other)
  {
    std::lock_guard<std::mutex> lock{other._mtx};
    _queue = other._queue;
  }

  LockQueue& operator=(const LockQueue& other)
  {
    if (this == &other) return *this;
    std::scoped_lock<std::mutex, std::mutex> lock{this->_mtx, other._mtx};
    this->_queue = other._queue;
    if (!this->_queue.empty())
    {
      _cv.notify_all();
    }
    return *this;
  }

  LockQueue(LockQueue&& other) noexcept(std::is_move_constructible_v<std::queue<T>>)
  {
    std::lock_guard<std::mutex> lock{other._mtx};
    _queue = std::move(other._queue);
  }

  LockQueue& operator=(LockQueue&& other) noexcept(std::is_move_assignable_v<std::queue<T>>)
  {
    if (this == &other) return *this;
    std::scoped_lock<std::mutex, std::mutex> lock{this->_mtx, other._mtx};
    this->_queue = std::move(other._queue);
    if (!this->_queue.empty())
    {
      _cv.notify_all();
    }
    return *this;
  }

  void push(const T& value)
  {
    {
      std::lock_guard<std::mutex> lock{_mtx};
      _queue.push(value);
    }
    _cv.notify_one();
  }

  void push(T&& value)
  {
    {
      std::lock_guard<std::mutex> lock{_mtx};
      _queue.push(std::move(value));
    }
    _cv.notify_one();
  }

  template <typename... Args>
  void emplace(Args&&... args)
  {
    {
      std::lock_guard<std::mutex> lock{_mtx};
      _queue.emplace(std::forward<Args>(args)...);
    }
    _cv.notify_one();
  }

  bool try_pop(T& value)
  {
    std::lock_guard<std::mutex> lock{_mtx};
    if (_queue.empty()) return false;
    value = std::move_if_noexcept(_queue.front());
    _queue.pop();
    return true;
  }

  std::optional<T> pop() noexcept
  {
    static_assert(std::is_nothrow_move_constructible_v<T>, "this operation need T contains no throw in move cons");
    std::unique_lock<std::mutex> lock{_mtx};
    _cv.wait(lock, [this]() -> bool { return !_queue.empty(); });
    std::optional<T> result{std::move(_queue.front())};
    _queue.pop();
    return result;
  }

  bool empty() const noexcept
  {
    std::lock_guard<std::mutex> lock{_mtx};
    return _queue.empty();
  }

  size_t size() const noexcept
  {
    std::lock_guard<std::mutex> lock{_mtx};
    return _queue.size();
  }

  void clear() noexcept
  {
    std::lock_guard<std::mutex> lock{_mtx};
    while (!_queue.empty())
    {
      _queue.pop();
    }
  }

private:
  std::queue<T> _queue;
  mutable std::mutex _mtx;
  std::condition_variable _cv;
};

template <typename T>
class LockQueueInList
{
public:
  LockQueueInList() : _head(new Node), _tail(_head), _size(0)
  {
  }

  ~LockQueueInList()
  {
    while (Node* const old_head = _head)
    {
      _head = _head->_next;
      delete old_head;
    }
  }

  LockQueueInList(const LockQueueInList&) = delete;
  LockQueueInList& operator=(const LockQueueInList&) = delete;

  void push(T item)
  {
    Node* const new_node = new Node();

    {
      std::lock_guard<std::mutex> lock{_tail_mtx};
      _tail->_data = std::move(item);
      _tail->_next = new_node;
      _tail = _tail->_next;
      _size.fetch_add(1, std::memory_order_acq_rel);
    }

    _size.notify_one();
  }

  std::optional<T> try_pop()
  {
    std::lock_guard<std::mutex> lock{_head_mtx};
    if (empty())
    {
      return std::nullopt;
    }
    return pop_head();
  }

  std::optional<T> pop()
  {
    std::unique_lock<std::mutex> lock{_head_mtx};
    while (true)
    {
      if (_size.load(std::memory_order_acquire) > 0)
      {
        return pop_head();
      }
      lock.unlock();
      _size.wait(0, std::memory_order_acquire);
      lock.lock();
    }
  }

  bool empty() const
  {
    return _size.load(std::memory_order_acquire) == 0;
  }

private:
  struct Node
  {
    std::optional<T> _data;
    Node* _next;

    Node() : _next(nullptr)
    {
    }
  };

  std::optional<T> pop_head()
  {
    Node* const old_head = _head;
    _head = old_head->_next;
    std::optional<T> result = std::move(old_head->_data);

    delete old_head;
    _size.fetch_sub(1, std::memory_order_acq_rel);

    return result;
  }

  Node* _head;
  Node* _tail;
  std::atomic<size_t> _size;
  mutable std::mutex _head_mtx;
  mutable std::mutex _tail_mtx;
};
