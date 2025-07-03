#include "Server.hpp"
#include "middleware/Logger.hpp"

#include <boost/asio/ip/address_v4.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/system/detail/error_code.hpp>

namespace core {

Server::Server(boost::asio::io_context &ioc, unsigned short port)
    : _ioc(ioc), _accep(_ioc, boost::asio::ip::tcp::endpoint(
                                  boost::asio::ip::address_v4::any(), port)) {
  logger.info("Server started, listening on port {}", port);
  start_accept();
}

void Server::start_accept() {
  auto* new_sess = new Session(_ioc);
  _accep.async_accept(new_sess->getSocket(), [this, new_sess](boost::system::error_code err) -> void {
    handle_accept(new_sess, err);
  });
}

void Server::handle_accept(Session* new_sess, const boost::system::error_code& err) {
  if (!err) {
    new_sess->Start();
  } else {
    logger.error("error code is: {}, error msg is: {}\n", err.value(), err.message());
    delete new_sess;
    new_sess = nullptr;
  }

  start_accept();
}

} // namespace core