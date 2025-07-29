/******************************************************************************
 *
 * @file       IoPool.hpp
 * @brief      io_context的池子，实现各个线程各跑一个的效果
 *
 * @author     KBchulan
 * @date       2025/07/28
 * @history
 ******************************************************************************/

#ifndef IOPOOL_HPP
#define IOPOOL_HPP

#include <memory>
#include <thread>

#include <core/CoreExport.hpp>
#include <global/Singleton.hpp>

#include <boost/asio/io_context.hpp>

namespace core {

class CORE_EXPORT IoPool final : public global::Singleton<IoPool> {
  friend class global::Singleton<IoPool>;

private:
  IoPool(unsigned int size = std::thread::hardware_concurrency());

public:
  ~IoPool();

  boost::asio::io_context &getIoContext();

private:
  struct _impl;
  std::unique_ptr<_impl> _pimpl;
};

} // namespace core

#define ioPool core::IoPool::getInstance()

#endif // IOPOOL_HPP