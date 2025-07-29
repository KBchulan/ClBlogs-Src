#include "Session.hpp"

#include <mutex>
#include <queue>
#include <atomic>
#include <memory>
#include <cstddef>

#include <global/Global.hpp>
#include <middleware/Logger.hpp>
#include <core/server/Server.hpp>
#include <core/logic/LogicNode.hpp>
#include <core/msg-node/MsgNode.hpp>
#include <core/logic/LogicSystem.hpp>

#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/random_generator.hpp>

#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/system/system_error.hpp>
#include <boost/system/detail/error_code.hpp>
#include <boost/asio/detail/socket_holder.hpp>

namespace core {

struct Session::_impl {
  boost::asio::io_context &_ioc;
  Server *_server;

  boost::asio::ip::tcp::socket _socket;
  std::string _uuid;

  std::atomic_bool _isClosed{false};

  std::shared_ptr<MsgNode> _recv_head_node;
  std::shared_ptr<RecvNode> _recv_body_node;

  std::mutex _send_mtx;
  std::queue<std::shared_ptr<SendNode>> _send_queue;

  _impl(boost::asio::io_context &ioc, Server *server)
      : _ioc(ioc), _server(server), _socket(ioc) {
    boost::uuids::uuid uuid = boost::uuids::random_generator_mt19937()();
    _uuid = boost::uuids::to_string(uuid);

    _recv_head_node = std::make_shared<MsgNode>(MSG_HEAD_TOTAL_LEN);
  }

  void close() {
    bool expected = false;

    if (_isClosed.compare_exchange_strong(expected, true, std::memory_order_acquire)) {
      if (_socket.is_open()) {
        _socket.close();
      }

      if (_server != nullptr) {
        _server->removeSession(_uuid);
      }
    }
  }
};

Session::Session(boost::asio::io_context &ioc, Server *server)
  : _pimpl(std::make_unique<_impl>(ioc, server)) {}

Session::~Session() = default;

void Session::Read() {
  boost::asio::co_spawn(_pimpl->_ioc, [self = shared_from_this()]() -> boost::asio::awaitable<void> {
    try {
      while (!self->_pimpl->_isClosed) {
        self->_pimpl->_recv_head_node->Clear();
        co_await boost::asio::async_read(self->_pimpl->_socket,
          boost::asio::buffer(self->_pimpl->_recv_head_node->_data, MSG_HEAD_TOTAL_LEN),
          boost::asio::use_awaitable);

        // 解析接收到的数据
        short msgType = 0;
        memcpy(&msgType, self->_pimpl->_recv_head_node->_data, MSG_TYPE_LENGTH);
        msgType = (short)boost::asio::detail::socket_ops::network_to_host_short(static_cast<u_short>(msgType));
        short msgLen = 0;
        memcpy(&msgLen, self->_pimpl->_recv_head_node->_data + MSG_TYPE_LENGTH, MSG_LEN_LENGTH);
        msgLen = (short)boost::asio::detail::socket_ops::network_to_host_short(static_cast<u_short>(msgLen));

        if (msgLen > MSG_BODY_LENGTH) {
          logger.error("Received message length exceeds maximum allowed length");
          self->_pimpl->close();
          co_return;
        }

        logger.info("Received message type: {}, length: {}", msgType, msgLen);

        // 读取消息内容
        self->_pimpl->_recv_body_node = std::make_shared<RecvNode>(msgType, msgLen);
        self->_pimpl->_recv_body_node->Clear();
        co_await boost::asio::async_read(self->_pimpl->_socket,
            boost::asio::buffer(self->_pimpl->_recv_body_node->_data, static_cast<size_t>(msgLen)),
            boost::asio::use_awaitable);

        // 投递到逻辑线程处理
        logicSystem.PostMsgToLogicQueue(std::make_shared<LogicNode>(self, self->_pimpl->_recv_body_node));
      }
    } catch (const boost::system::system_error &err) {
      logger.error("Session receive error: {}", err.code().message());
      self->_pimpl->close();
    }
  }, boost::asio::detached);
}

void Session::Send(short msgType, short msgLen, const char *msgBody) {
  auto send_node = std::make_shared<SendNode>(msgType, msgLen, msgBody);
  bool should_start_coroutine = false;

  {
    std::lock_guard<std::mutex> lock(_pimpl->_send_mtx);
    should_start_coroutine = _pimpl->_send_queue.empty();
    if (_pimpl->_send_queue.size() >= SEND_QUEUE_MAX_LEN) {
      logger.error("Send queue is full, dropping message");
      return;
    }
    _pimpl->_send_queue.emplace(send_node);
  }

  if (should_start_coroutine) {
    boost::asio::co_spawn(_pimpl->_ioc, [self = shared_from_this()]() -> boost::asio::awaitable<void> {
      try {
        while (true) {
          std::shared_ptr<SendNode> node;
          {
            std::lock_guard<std::mutex> lock{self->_pimpl->_send_mtx};
            if (self->_pimpl->_send_queue.empty()) {
              co_return;
            }
            node = std::move(self->_pimpl->_send_queue.front());
            self->_pimpl->_send_queue.pop();
          }
          co_await boost::asio::async_write(self->_pimpl->_socket,
            boost::asio::buffer(node->_data, static_cast<size_t>(node->_msg_len)),
            boost::asio::use_awaitable);
          }
      } catch (const boost::system::system_error &err) {
        logger.error("Session send error: {}", err.code().message());
        self->_pimpl->close();
      }
    }, boost::asio::detached);
  }

}

std::string &Session::getUuid() const {
  return _pimpl->_uuid;
}

boost::asio::ip::tcp::socket &Session::getSocket() {
  return _pimpl->_socket;
}

} // namespace core