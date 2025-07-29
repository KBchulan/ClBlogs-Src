/******************************************************************************
 *
 * @file       Server.hpp
 * @brief      服务器的核心实现
 *
 * @author     KBchulan
 * @date       2025/07/28
 * @history
 ******************************************************************************/

#ifndef SERVER_HPP
#define SERVER_HPP

#include <memory>

#include <core/CoreExport.hpp>

#include <boost/asio/io_context.hpp>

namespace core {

class Session;
class CORE_EXPORT Server {
public:
  Server(boost::asio::io_context &ioc, unsigned short port);

  ~Server();

  void removeSession(const std::string &key);

private:
  struct _impl;
  std::unique_ptr<_impl> _pimpl;
};

} // namespace core

#endif // SERVER_HPP