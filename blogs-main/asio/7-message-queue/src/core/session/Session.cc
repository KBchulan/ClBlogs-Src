#include "Session.hpp"

#include <memory>
#include <mutex>
#include <cstddef>

#include <boost/asio/write.hpp>

#include <core/msg-node/MsgNode.hpp>
#include <core/server/Server.hpp>
#include <middleware/Logger.hpp>

namespace core {

void Session::Start() {
  _data.fill('\0');
  _sock.async_read_some(boost::asio::buffer(_data.data(), MAX_LENGTH),
    [self = shared_from_this()](const boost::system::error_code &err, std::size_t bytes_transferred) -> void {
      self->handle_read(err, bytes_transferred);
    }
  );
}

void Session::Send(char *data, size_t leng) {
  bool pending = false;
  std::shared_ptr<MsgNode> node = std::make_shared<MsgNode>(data, leng);

  {
    std::scoped_lock<std::mutex> lock{_send_mtx};
    pending = _send_queue.empty();
    _send_queue.emplace(node);
  }

  if (!pending) {
    return;
  }

  boost::asio::async_write(_sock, boost::asio::buffer(data, leng),
    [self = shared_from_this()](const boost::system::error_code& errc, size_t) -> void {
      self->handle_write(errc);
    }
  );
}

void Session::handle_read(const boost::system::error_code& err, std::size_t bytes_transferred) {
  if (!err) {
    logger.info("receive data: {}\n", std::string(_data.data(), bytes_transferred));
    Send(_data.data(), bytes_transferred);

    _data.fill('\0');
    _sock.async_read_some(boost::asio::buffer(_data.data(), MAX_LENGTH),
      [self = shared_from_this()](const boost::system::error_code& erro, size_t bytes) -> void {
        self->handle_read(erro, bytes);
      }
    );
  } else {
    logger.error("read error, err msg is: {}\n", err.message());
  }
}

void Session::handle_write(const boost::system::error_code& err) {
  if (!err) {
    std::scoped_lock<std::mutex> lock{_send_mtx};
    _send_queue.pop();
    if (!_send_queue.empty()) {
      auto& node =_send_queue.front();
      boost::asio::async_write(_sock, boost::asio::buffer(node->_data, node->_max_len),
        [self = shared_from_this()](const boost::system::error_code& errc, size_t) -> void {
          self->handle_write(errc);
        }
      );
    }
  } else {
    logger.error("write error, err msg is: {}\n", err.message());
  }
}

} // namespace core