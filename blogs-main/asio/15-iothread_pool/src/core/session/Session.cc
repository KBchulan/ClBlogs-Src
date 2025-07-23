#include "Session.hpp"

#include <cstddef>
#include <memory>
#include <cstring>
#include <winsock2.h>

#include <json/json.h>
#include <json/writer.h>

#include <boost/asio/write.hpp>
#include <boost/asio/detail/socket_holder.hpp>

#include <middleware/Logger.hpp>
#include <core/server/Server.hpp>
#include <core/logic/LogicNode.hpp>
#include <core/msg-node/MsgNode.hpp>
#include <core/logic/LogicSystem.hpp>

namespace core {

void Session::Start() {
  _recv_head_node = std::make_shared<MsgNode>(MSG_HEAD_TOTAL_LEN);

  memset(_data.data(), 0, std::min(static_cast<size_t>(MSG_BODY_LENGTH), _data.size()));
  _sock.async_read_some(boost::asio::buffer(_data.data(), MSG_BODY_LENGTH),
    [self = shared_from_this()](const boost::system::error_code &err, std::size_t bytes_transferred) -> void {
      self->handle_read(err, bytes_transferred);
    }
  );
}

void Session::Close() {
  bool expected = false;
  if (_is_closed.compare_exchange_strong(expected, true)) {
    if (_sock.is_open()) {
      boost::system::error_code errc;
      _sock.close(errc);
      if (errc) {
        logger.warning("Socket close error: {}", errc.message());
      }
    }
    _server->RemoveSession(_uuid);
  }
}

void Session::Send(short msg_id, short msg_len, const char *data) {
  bool pending = false;
  std::shared_ptr<MsgNode> node = std::make_shared<SendNode>(msg_id, msg_len, data);

  {
    std::scoped_lock<std::mutex> lock{_send_mtx};
    pending = _send_queue.empty();
    if (_send_queue.size() >= SEND_QUEUE_MAX_LEN){
      logger.error("the send queue is too long, don't push new bag");
      return;
    }
    _send_queue.emplace(node);
  }

  if (!pending) {
    return;
  }

  auto& msg_node = _send_queue.front();
  boost::asio::async_write(_sock, boost::asio::buffer(msg_node->_data, static_cast<size_t>(msg_node->_msg_len)),
    [self = shared_from_this()](const boost::system::error_code& errc, size_t) -> void {
      self->handle_write(errc);
    }
  );
}

void Session::handle_read(const boost::system::error_code &err, std::size_t bytes_transferred) {
  if (!err) {
    size_t copy_len = 0;
    while (bytes_transferred > 0) {
      // 处理头部
      if (!_head_parse) {
        // 收到的数据还没有头部长度
        if (bytes_transferred + static_cast<size_t>(_recv_head_node->_cur_len) < MSG_HEAD_TOTAL_LEN) {
          memcpy(_recv_head_node->_data + _recv_head_node->_cur_len, _data.data() + copy_len, bytes_transferred);
          _recv_head_node->_cur_len += bytes_transferred;

          memset(_data.data(), 0, std::min(static_cast<size_t>(MSG_BODY_LENGTH), _data.size()));
          _sock.async_read_some(boost::asio::buffer(_data.data(), MSG_BODY_LENGTH),
          [self = shared_from_this()](const boost::system::error_code &errc, size_t bytes) -> void {
            self->handle_read(errc, bytes);
          });
          return;
        }

        // 再次处理剩余的头部信息
        auto head_remain = static_cast<size_t>(MSG_HEAD_TOTAL_LEN - _recv_head_node->_cur_len);
        memcpy(_recv_head_node->_data + _recv_head_node->_cur_len, _data.data() + copy_len, head_remain);

        // 更新处理完头部剩余的内容
        copy_len += head_remain;
        bytes_transferred -= head_remain;

        // 处理数据部分，先获取数据部分的id和长度
        short data_id = 0;
        short data_len = 0;
        memcpy(&data_id, _recv_head_node->_data, MSG_TYPE_LENGTH);
        memcpy(&data_len, _recv_head_node->_data + MSG_TYPE_LENGTH, MSG_LEN_LENGTH);
        data_id = (short)boost::asio::detail::socket_ops::network_to_host_short(static_cast<u_short>(data_id));
        data_len = (short)boost::asio::detail::socket_ops::network_to_host_short(static_cast<u_short>(data_len));
        logger.info("receive data id is: {}, data len is: {}", data_id, data_len);

        if (data_id < 0 || data_id > 2000) {
          logger.error("Invalid msg id, id is: {}", data_id);
          Close();
          return;
        }
        if (data_len > MSG_BODY_LENGTH) {
          logger.error("Too long msg received, len is: {}", data_len);
          Close();
          return;
        }

        // 创建消息节点
        _recv_msg_node = std::make_shared<RecvNode>(data_id, data_len);

        // 如果消息没有接收全部
        if (bytes_transferred < static_cast<size_t>(data_len)) {
          memcpy(_recv_msg_node->_data + _recv_msg_node->_cur_len, _data.data() + copy_len, bytes_transferred);
          _recv_msg_node->_cur_len += bytes_transferred;

          memset(_data.data(), 0, std::min(static_cast<size_t>(MSG_BODY_LENGTH), _data.size()));
          _sock.async_read_some(boost::asio::buffer(_data.data(), MSG_BODY_LENGTH),
          [self = shared_from_this()](const boost::system::error_code &errc, size_t bytes) -> void {
            self->handle_read(errc, bytes);
          });
          _head_parse.store(true, std::memory_order_release);
          return;
        }

        // 如果接收到的数据更长的话，则先接收当前节点的数据
        memcpy(_recv_msg_node->_data + _recv_msg_node->_cur_len, _data.data() + copy_len, static_cast<size_t>(data_len));

        _recv_msg_node->_cur_len += data_len;
        copy_len += static_cast<size_t>(data_len);
        bytes_transferred -= static_cast<size_t>(data_len);
        _recv_msg_node->_data[_recv_msg_node->_msg_len] = '\0';

        logicSystem.PostMsgToLogicQueue(std::make_shared<LogicNode>(shared_from_this(), std::dynamic_pointer_cast<RecvNode>(_recv_msg_node)));

        // 处理剩余的数据
        _head_parse.store(false, std::memory_order_release);
        memset(_recv_head_node->_data, 0, MSG_HEAD_TOTAL_LEN);

        if (bytes_transferred <= 0) {
          memset(_data.data(), 0, std::min(static_cast<size_t>(MSG_BODY_LENGTH), _data.size()));
          _sock.async_read_some(boost::asio::buffer(_data.data(), MSG_BODY_LENGTH),
            [self = shared_from_this()](const boost::system::error_code& eee, size_t bbb) -> void {
              self->handle_read(eee, bbb);
            }
          );
          return;
        }

        continue;
      }

      // 处理完头部且剩余部分不足总长度return分支之后
      auto msg_remain = static_cast<size_t>(_recv_msg_node->_msg_len - _recv_msg_node->_cur_len);
      // 数据不够长的情况下
      if (bytes_transferred < msg_remain) {
        memcpy(_recv_msg_node->_data + _recv_msg_node->_cur_len, _data.data() + copy_len, bytes_transferred);
        _recv_msg_node->_cur_len += bytes_transferred;

        memset(_data.data(), 0, std::min(static_cast<size_t>(MSG_BODY_LENGTH), _data.size()));
        _sock.async_read_some(boost::asio::buffer(_data.data(), MSG_BODY_LENGTH),
        [self = shared_from_this()](const boost::system::error_code &errc, size_t bytes) -> void {
          self->handle_read(errc, bytes);
        });
        return;
      }

      memcpy(_recv_msg_node->_data + _recv_msg_node->_cur_len, _data.data() + copy_len, msg_remain);

      _recv_msg_node->_cur_len += msg_remain;
      bytes_transferred -= msg_remain;
      copy_len += msg_remain;
      _recv_msg_node->_data[_recv_msg_node->_msg_len] = '\0';

      logicSystem.PostMsgToLogicQueue(std::make_shared<LogicNode>(shared_from_this(), std::dynamic_pointer_cast<RecvNode>(_recv_msg_node)));

      // 处理剩余的数据
      _head_parse.store(false, std::memory_order_release);
      memset(_recv_head_node->_data, 0, static_cast<size_t>(_recv_head_node->_msg_len));

      if (bytes_transferred <= 0) {
        memset(_data.data(), 0, std::min(static_cast<size_t>(MSG_BODY_LENGTH), _data.size()));
        _sock.async_read_some(boost::asio::buffer(_data.data(), MSG_BODY_LENGTH),
          [self = shared_from_this()](const boost::system::error_code& eee, size_t bbb) -> void {
            self->handle_read(eee, bbb);
          }
        );
        return;
      }
    }
  } else {
    logger.error("read error, err msg is: {}", err.message());
    Close();
  }
}

void Session::handle_write(const boost::system::error_code& err) {
  if (!err) {
    std::scoped_lock<std::mutex> lock{_send_mtx};
    _send_queue.pop();
    if (!_send_queue.empty()) {
      auto& node =_send_queue.front();
      boost::asio::async_write(_sock, boost::asio::buffer(node->_data, static_cast<size_t>(node->_msg_len)),
        [self = shared_from_this()](const boost::system::error_code& errc, size_t) -> void {
          self->handle_write(errc);
        }
      );
    }
  } else {
    logger.error("write error, err msg is: {}", err.message());
    Close();
  }
}

} // namespace core