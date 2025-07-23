#include "IOPool.hpp"

#include <vector>

#include <middleware/Logger.hpp>
#include <boost/asio/executor_work_guard.hpp>

namespace core {

struct IOPool::_impl {
  boost::asio::io_context _ioc;
  boost::asio::executor_work_guard<boost::asio::io_context::executor_type> _work_guard;
  std::vector<std::jthread> _threads;

  _impl(std::size_t threadCount) : _work_guard(boost::asio::make_work_guard(_ioc)) {
    // 创建所有的线程
    _threads.reserve(threadCount);
    for (std::size_t i = 0; i < threadCount; ++i) {
      _threads.emplace_back([this]() -> void {
          _ioc.run();
      });
    }
  }
};

IOPool::IOPool(std::size_t threadCount) : _pimpl(std::make_unique<_impl>(threadCount)) { }

IOPool::~IOPool() { }

boost::asio::io_context &IOPool::getIOContext() {
  return _pimpl->_ioc;
}

} // namespace core