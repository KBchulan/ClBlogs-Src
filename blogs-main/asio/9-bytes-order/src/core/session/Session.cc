#include "Session.hpp"

#include <cstddef>
#include <cstring>

#include <boost/asio/write.hpp>
#include <boost/asio/detail/socket_holder.hpp>

#include <core/msg-node/MsgNode.hpp>
#include <core/server/Server.hpp>
#include <middleware/Logger.hpp>
#include <winsock2.h>

namespace core {

void Session::Start() {
  _head_parse.store(false, std::memory_order_release);
  _recv_head_node = std::make_shared<MsgNode>(HEAD_LENGTH);

  _data.fill('\0');
  _sock.async_read_some(boost::asio::buffer(_data.data(), MAX_LENGTH),
    [self = shared_from_this()](const boost::system::error_code &err, std::size_t bytes_transferred) -> void {
      self->handle_read(err, bytes_transferred);
    }
  );
}

void Session::Close() {
  _sock.close();
}

void Session::Send(char *data, size_t leng) {
  bool pending = false;
  std::shared_ptr<MsgNode> node = std::make_shared<MsgNode>(data, leng);

  {
    std::scoped_lock<std::mutex> lock{_send_mtx};
    pending = _send_queue.empty();
    if (_send_queue.size() >= SEND_QUEUE_MAX_LEN){
      logger.error("the send queue is too long, don't push new bag\n");
      return;
    }
    _send_queue.emplace(node);
  }

  if (!pending) {
    return;
  }

  auto& msg_node = _send_queue.front();
  boost::asio::async_write(_sock, boost::asio::buffer(msg_node->_data, static_cast<size_t>(msg_node->_max_len)),
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
        if (bytes_transferred + static_cast<size_t>(_recv_head_node->_cur_len) < HEAD_LENGTH) {
          memcpy(_recv_head_node->_data + _recv_head_node->_cur_len, _data.data() + copy_len, bytes_transferred);
          _recv_head_node->_cur_len += bytes_transferred;
          memset(_data.data(), 0, MAX_LENGTH);

          _sock.async_read_some(boost::asio::buffer(_data.data(), MAX_LENGTH),
          [self = shared_from_this()](const boost::system::error_code &errc, size_t bytes) -> void {
            self->handle_read(errc, bytes);
          });
          return;
        }

        // 再次处理剩余的头部信息
        auto head_remain = static_cast<size_t>(HEAD_LENGTH - _recv_head_node->_cur_len);
        memcpy(_recv_head_node->_data + _recv_head_node->_cur_len, _data.data() + copy_len, head_remain);
        _recv_head_node->_data[_recv_head_node->_max_len] = '\0';

        // 更新处理完头部剩余的内容
        copy_len += head_remain;
        bytes_transferred -= head_remain;

        // 处理数据部分，先获取数据部分的长度
        short data_len = 0;
        memcpy(&data_len, _recv_head_node->_data, HEAD_LENGTH);
        data_len = (short)boost::asio::detail::socket_ops::network_to_host_short(static_cast<u_short>(data_len));
        logger.info("receive data len is: {}\n", data_len);

        if (data_len > MAX_LENGTH) {
          logger.error("too long msg received, len is: {}\n", data_len);
          return;
        }

        // 创建消息节点
        _recv_msg_node = std::make_shared<MsgNode>(data_len);

        // 如果消息没有接收全部
        if (bytes_transferred < static_cast<size_t>(data_len)) {
          memcpy(_recv_msg_node->_data + _recv_msg_node->_cur_len, _data.data() + copy_len, bytes_transferred);
          _recv_msg_node->_cur_len += bytes_transferred;
          memset(_data.data(), 0, MAX_LENGTH);

          _sock.async_read_some(boost::asio::buffer(_data.data(), MAX_LENGTH),
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
        _recv_msg_node->_data[_recv_msg_node->_max_len] = '\0';
        std::cout << std::format("receive data is: {}\n", _recv_msg_node->_data);

        // 至此，分支1的接收逻辑走完了，调用Send测试一下
        Send(_recv_msg_node->_data, static_cast<size_t>(_recv_msg_node->_max_len));

        // 处理剩余的数据
        _head_parse.store(false, std::memory_order_release);
        memset(_recv_head_node->_data, 0, static_cast<size_t>(_recv_head_node->_max_len));

        if (bytes_transferred <= 0) {
          memset(_data.data(), 0, MAX_LENGTH);
          _sock.async_read_some(boost::asio::buffer(_data.data(), MAX_LENGTH),
            [self = shared_from_this()](const boost::system::error_code& eee, size_t bbb) -> void {
              self->handle_read(eee, bbb);
            }
          );
          return;
        }

        continue;
      }

      // 处理完头部且剩余部分不足总长度return分支之后
      auto msg_remain = static_cast<size_t>(_recv_msg_node->_max_len - _recv_msg_node->_cur_len);
      // 数据不够长的情况下
      if (bytes_transferred < msg_remain) {
        memcpy(_recv_msg_node->_data + _recv_msg_node->_cur_len, _data.data() + copy_len, bytes_transferred);
        _recv_msg_node->_cur_len += bytes_transferred;
        memset(_data.data(), 0, MAX_LENGTH);

        _sock.async_read_some(boost::asio::buffer(_data.data(), MAX_LENGTH),
        [self = shared_from_this()](const boost::system::error_code &errc, size_t bytes) -> void {
          self->handle_read(errc, bytes);
        });
        return;
      }

      memcpy(_recv_msg_node->_data + _recv_msg_node->_cur_len, _data.data() + copy_len, msg_remain);
      _recv_msg_node->_cur_len += msg_remain;
      bytes_transferred -= msg_remain;
      copy_len += msg_remain;
      _recv_msg_node->_data[_recv_msg_node->_max_len] = '\0';
      std::cout << std::format("receive data is: {}\n", _recv_msg_node->_data);
      // 至此，分支2的接收逻辑走完了，调用Send测试一下
      Send(_recv_msg_node->_data, static_cast<size_t>(_recv_msg_node->_max_len));

      // 处理剩余的数据
      _head_parse.store(false, std::memory_order_release);
      memset(_recv_head_node->_data, 0, static_cast<size_t>(_recv_head_node->_max_len));

      if (bytes_transferred <= 0) {
        memset(_data.data(), 0, MAX_LENGTH);
        _sock.async_read_some(boost::asio::buffer(_data.data(), MAX_LENGTH),
          [self = shared_from_this()](const boost::system::error_code& eee, size_t bbb) -> void {
            self->handle_read(eee, bbb);
          }
        );
        return;
      }
    }
  } else {
    logger.error("read error, err msg is: {}\n", err.message());
    _server->RemoveSession(_uuid);
    Close();
  }
}

void Session::handle_write(const boost::system::error_code& err) {
  if (!err) {
    std::scoped_lock<std::mutex> lock{_send_mtx};
    logger.info("send data is: {}\n", _send_queue.front()->_data + HEAD_LENGTH);
    _send_queue.pop();
    if (!_send_queue.empty()) {
      auto& node =_send_queue.front();
      boost::asio::async_write(_sock, boost::asio::buffer(node->_data, static_cast<size_t>(node->_max_len)),
        [self = shared_from_this()](const boost::system::error_code& errc, size_t) -> void {
          self->handle_write(errc);
        }
      );
    }
  } else {
    logger.error("write error, err msg is: {}\n", err.message());
    _server->RemoveSession(_uuid);
    Close();
  }
}

} // namespace core