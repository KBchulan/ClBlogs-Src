#include "Server.hpp"

#include <mutex>
#include <memory>
#include <unordered_map>

#include <middleware/Logger.hpp>
#include <core/io-pool/IoPool.hpp>
#include <core/session/Session.hpp>

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/system/detail/error_code.hpp>

namespace core {

struct Server::_impl {
  boost::asio::io_context &_ioc;
  unsigned short _port;
  Server * _server;

  boost::asio::ip::tcp::acceptor _acceptor;

  std::mutex _mutex;
  std::unordered_map<std::string, std::shared_ptr<Session>> _sessions;

  void start_accept() {
    auto &ioc = ioPool.getIoContext();
    auto new_session = std::make_shared<Session>(ioc, _server);
    _acceptor.async_accept(new_session->getSocket(), [new_session, this](boost::system::error_code errc) -> void {
      handle_accept(new_session, errc);
    });
  }

  void handle_accept(const std::shared_ptr<Session> &new_session, const boost::system::error_code &errc) {
    if (errc) {
      logger.error("Accept error: {}", errc.message());
    } else {
      new_session->Read();
      std::lock_guard<std::mutex> lock(_mutex);
      _sessions[new_session->getUuid()] = new_session;
    }

    start_accept();
  }

  _impl(boost::asio::io_context &ioc, unsigned short port, Server *server)
    : _ioc(ioc), _port(port), _server(server), _acceptor(_ioc, boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4::any(), port)) {
    logger.info("Server is starting on port {}", port);
    start_accept();
  }
};

Server::Server(boost::asio::io_context &ioc, unsigned short port)
  : _pimpl(std::make_unique<_impl>(ioc, port, this)) {}

Server::~Server() {
  logger.debug("The server has been released!");
}

void Server::removeSession(const std::string &key) {
  std::lock_guard<std::mutex> lock(_pimpl->_mutex);
  if (auto iter = _pimpl->_sessions.find(key); iter != _pimpl->_sessions.end()) {
    _pimpl->_sessions.erase(iter);
    logger.info("Session with key {} has been removed", key);
  } else {
    logger.warning("Session with key {} not found", key);
  }
}

} // namespace core