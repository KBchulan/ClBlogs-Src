#ifndef SERVER_HPP
#define SERVER_HPP

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <boost/system/detail/error_code.hpp>
#include <core/CoreExport.hpp>
#include <core/session/Session.hpp>

namespace core {

class CORE_EXPORT Server {
public:
  Server(boost::asio::io_context& ioc, unsigned short port);

private:
  void start_accept();
  void handle_accept(Session* new_sess, const boost::system::error_code& err);

  boost::asio::io_context &_ioc;
  boost::asio::ip::tcp::acceptor _accep;
};

} // namespace core

#endif // SERVER_HPP