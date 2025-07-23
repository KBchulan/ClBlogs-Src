#ifndef IOPOOL_HPP
#define IOPOOL_HPP

#include <thread>
#include <memory>
#include <cstddef>

#include <boost/asio/io_context.hpp>

#include <core/CoreExport.hpp>
#include <global/Singleton.hpp>

namespace core {

class CORE_EXPORT IOPool final : public global::Singleton<IOPool> {
  friend class global::Singleton<IOPool>;

private:
  IOPool(std::size_t threadCount = std::thread::hardware_concurrency());

public:
  ~IOPool();

  boost::asio::io_context &getIOContext();

private:
  struct _impl;
  std::unique_ptr<_impl> _pimpl;
};

} // namespace core

#define ioPool core::IOPool::getInstance()

#endif // IOPOOL_HPP