#include "IOPool.hpp"

#include <atomic>
#include <vector>

#include <boost/asio/executor_work_guard.hpp>

namespace core {

struct IOPool::_impl {
  std::vector<boost::asio::io_context> _ioContexts;
  std::vector<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>> _workGuards;

  std::vector<std::jthread> _threads;
  std::atomic<std::size_t> _nextIndex{0};

  _impl(std::size_t size) : _ioContexts(size) {
    // 创建各个ioc的work_guard，防止提早退出
    _workGuards.reserve(size);
    for (auto &ioc : _ioContexts) {
      _workGuards.emplace_back(boost::asio::make_work_guard(ioc));
    }

    // 创建各个线程，每个线程自己跑一个ioc
    _threads.reserve(size);
    for (auto &ioc : _ioContexts) {
      _threads.emplace_back([&ioc]() -> void {
        ioc.run();
      });
    }
  }
};

IOPool::IOPool(std::size_t size) : _pimpl(std::make_unique<_impl>(size)) { }

IOPool::~IOPool() {}

boost::asio::io_context &IOPool::getIocontext() {
  const std::size_t poolSize = _pimpl->_ioContexts.size();
  const std::size_t index = _pimpl->_nextIndex.fetch_add(1, std::memory_order_relaxed) % poolSize;
  return _pimpl->_ioContexts[index];
}

} // namespace core