/******************************************************************************
 *
 * @file       Session.hpp
 * @brief      每个连接的会话类
 *
 * @author     KBchulan
 * @date       2025/07/28
 * @history
 ******************************************************************************/

#ifndef SESSION_HPP
#define SESSION_HPP

#include <memory>

#include <core/CoreExport.hpp>

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_context.hpp>

namespace core {

class Server;
class CORE_EXPORT Session : public std::enable_shared_from_this<Session>  {
public:
  Session(boost::asio::io_context &ioc, Server *server);

  ~Session();

  void Read();
  void Send(short msgType, short msgLen, const char *msgBody);

  std::string &getUuid() const;
  boost::asio::ip::tcp::socket &getSocket();

private:
  struct _impl;
  std::unique_ptr<_impl> _pimpl;
};

} // namespace core

#endif // SESSION_HPP