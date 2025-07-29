#include "IoPool.hpp"

#include <atomic>
#include <vector>
#include <cstddef>

#include <middleware/Logger.hpp>

#include <boost/asio/executor_work_guard.hpp>

namespace core {

struct IoPool::_impl {
  std::vector<std::jthread> _threads;
  std::vector<boost::asio::io_context> _ioContexts;
  std::vector<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>> _workGuards;
  std::atomic<size_t> _index{0};

  _impl(unsigned int size) : _ioContexts(size) {
    // 启动work_guard
    _workGuards.reserve(size);
    for (auto &io_context : _ioContexts) {
      _workGuards.emplace_back(boost::asio::make_work_guard(io_context));
    }

    // 启动线程
    _threads.reserve(size);
    for (auto &io_context : _ioContexts) {
      _threads.emplace_back([&io_context]() -> void {
        io_context.run();
      });
    }
  }

  ~_impl() = default;
};

IoPool::IoPool(unsigned int size) : _pimpl(std::make_unique<_impl>(size)) {}

IoPool::~IoPool() {
  logger.debug("The io_pool has been released!");
}

boost::asio::io_context &IoPool::getIoContext() {
  const std::size_t poolSize = _pimpl->_ioContexts.size();
  const std::size_t index = _pimpl->_index.fetch_add(1, std::memory_order_relaxed) % poolSize;
  return _pimpl->_ioContexts[index];
}

} // namespace core