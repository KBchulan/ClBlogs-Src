#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/core/buffers_to_string.hpp>
#include <boost/beast/core/error.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/core/stream_traits.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/websocket/stream.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/signal_set.hpp>

#include <cstddef>
#include <memory>
#include <mutex>
#include <print>
#include <queue>
#include <unordered_map>

class Connection;
class ConnectionMgr {
public:
  static ConnectionMgr &getInstance();
  void addConnection(std::shared_ptr<Connection> conn);
  void removeConnection(const std::string &uuid);

private:
  ConnectionMgr() = default;
  ~ConnectionMgr() = default;

  std::mutex _mtx;
  std::unordered_map<std::string, std::shared_ptr<Connection>> _connections;
};

class Connection : public std::enable_shared_from_this<Connection> {
public:
  Connection(boost::asio::io_context &ioc)
    : _ioc(ioc), _strand(boost::asio::make_strand(_ioc)), _ws_ptr(std::make_unique<boost::beast::websocket::stream<boost::beast::tcp_stream>>(_strand)) {
    boost::uuids::uuid uuid = boost::uuids::random_generator_mt19937()();
    _uuid = boost::uuids::to_string(uuid);
  }

  void asyncAccept() {
    _ws_ptr->async_accept(
      [self = shared_from_this()](boost::beast::error_code errc) -> void {
        if (!errc) {
          self->start();
          ConnectionMgr::getInstance().addConnection(self);
        } else {
          std::print("accept error, error is: {}\n", errc.message());
        }
      });
  }

  void start() {
    _ws_ptr->async_read(_recv_buffer,
      [self = shared_from_this()](boost::beast::error_code errc, std::size_t bytes_transferred) -> void {
        if (!errc) {
          self->_ws_ptr->text(self->_ws_ptr->got_text());
          std::string message(boost::beast::buffers_to_string(self->_recv_buffer.data()));
          self->_recv_buffer.consume(bytes_transferred);

          std::print("received message: {}\n", message);
          self->asyncSend("Echo: " + message);
          self->start();
        } else {
          std::print("read error, error is: {}\n", errc.message());
          ConnectionMgr::getInstance().removeConnection(self->getUuid());
        }
      }
    );
  }

  void asyncSend(std::string data) {
    int size;
    {
      std::lock_guard<std::mutex> lock(_send_mtx);
      size = _send_queue.size();
      _send_queue.push(std::move(data));
    }

    if (size >= 1) {
      return;
    }

    boost::asio::co_spawn(
      _strand,
      [self = shared_from_this()]() -> boost::asio::awaitable<void> {
        while (true) {
          std::string data;
          {
            std::lock_guard<std::mutex> lock(self->_send_mtx);
            if (self->_send_queue.empty()) {
              break;
            }
            data = std::move(self->_send_queue.front());
            self->_send_queue.pop();
          }

          try {
            co_await self->_ws_ptr->async_write(boost::asio::buffer(data), boost::asio::use_awaitable);
          } catch (boost::beast::system_error &e) {
            std::print("write error, error is: {}\n", e.what());
            ConnectionMgr::getInstance().removeConnection(self->getUuid());
            break;
          }
        }
      }, boost::asio::detached);
  }

  std::string &getUuid() {
    return this->_uuid;
  }

  boost::asio::ip::tcp::socket &getSocket() {
    return boost::beast::get_lowest_layer(*_ws_ptr).socket();
  }

private:
  std::string _uuid;
  boost::asio::io_context &_ioc;
  boost::asio::strand<boost::asio::io_context::executor_type> _strand;

  std::unique_ptr<boost::beast::websocket::stream<boost::beast::tcp_stream>> _ws_ptr;

  boost::beast::flat_buffer _recv_buffer;

  std::queue<std::string> _send_queue;
  std::mutex _send_mtx;
};

ConnectionMgr &ConnectionMgr::getInstance() {
  static ConnectionMgr instance;
  return instance;
}

void ConnectionMgr::addConnection(std::shared_ptr<Connection> conn) {
  std::lock_guard<std::mutex> lock(_mtx);
  _connections[conn->getUuid()] = conn;
}

void ConnectionMgr::removeConnection(const std::string &uuid) {
  std::lock_guard<std::mutex> lock(_mtx);
  _connections.erase(uuid);
}



class WebSocketServer {
public:
  WebSocketServer(boost::asio::io_context &ioc, const boost::asio::ip::tcp::endpoint &endpoint)
    : _ioc(ioc), _acceptor(ioc, endpoint) {
    std::print("WebSocket server started on port {}\n", endpoint.port());
    startAccept();
  }

  void startAccept() {
    auto conn = std::make_shared<Connection>(_ioc);
    _acceptor.async_accept(conn->getSocket(),
      [this, conn](boost::beast::error_code errc) {
        if (!errc) {
          conn->asyncAccept();
        } else {
          std::print("accept error, error is: {}\n", errc.message());
        }
        startAccept();
      });
  }

private:
  boost::asio::io_context &_ioc;
  boost::asio::ip::tcp::acceptor _acceptor;
};

int main() {
  try {
    boost::asio::io_context ioc;
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address_v4::any(), 10088);

    boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
    signals.async_wait([&ioc](const boost::system::error_code &ec, int signal) -> void {
      if (!ec) {
        std::print("Received signal {}, stopping server...\n", signal);
        ioc.stop();
      } else {
        std::print("Error waiting for signal: {}\n", ec.message());
      }
    });

    WebSocketServer server(ioc, endpoint);

    ioc.run();
  } catch (const std::exception &e) {
    std::print("Exception: {}\n", e.what());
  }
}