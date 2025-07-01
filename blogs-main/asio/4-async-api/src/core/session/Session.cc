#include "Session.hpp"
#include "core/msg-node/MsgNode.hpp"

#include <atomic>
#include <boost/asio.hpp>

#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <cstddef>
#include <iostream>
#include <utility>

namespace core {

Session::Session(std::shared_ptr<boost::asio::ip::tcp::socket> sock): _sock(std::move(sock)) {}

void Session::Connect(const boost::asio::ip::tcp::endpoint &enp) {
  try {
    _sock->connect(enp);
  } catch (const boost::system::system_error& error) {
    std::cout << std::format("error msg is: {}\n", error.code().message());
  }
}
// bind版本
// void Session::WriteToSocketErr(const std::string &str) {
//   _send_node = std::make_shared<MsgNode>(str.c_str(), str.length());
//   _sock->async_write_some(boost::asio::buffer(_send_node->_msg, _send_node->_totalLen),
//     std::bind(&Session::WriteToSocketCbErr, this, std::placeholders::_1, std::placeholders::_2, _send_node)
//   );
// }

// lambda版本
void Session::WriteToSocketErr(const std::string &str) {
  _send_node = std::make_shared<MsgNode>(str.c_str(), str.length());
  _sock->async_write_some(boost::asio::buffer(_send_node->_msg, _send_node->_totalLen),
    [this](const boost::system::error_code& errc, std::size_t bytes_transferred) -> void {
      WriteToSocketCbErr(errc, bytes_transferred);
    }
  );
}

void Session::WriteToSocketCbErr(const boost::system::error_code& errc, std::size_t bytes_transferred){
  if (bytes_transferred + _send_node->_curLen < _send_node->_totalLen && !errc) {
    _send_node->_curLen += bytes_transferred;
    _sock->async_write_some(
        boost::asio::buffer(_send_node->_msg + _send_node->_curLen, _send_node->_totalLen - _send_node->_curLen),
        [this](const boost::system::error_code &errc, std::size_t bytes_transferred) -> void {
          WriteToSocketCbErr(errc, bytes_transferred);
        });
  }
}

void Session::WriteToSocket(const std::string &str) {
  _queue.emplace(new MsgNode(str.c_str(), str.length()));
  if (sending) {
    return;
  }

  sending.store(true, std::memory_order_acquire);

  _sock->async_write_some(boost::asio::buffer(str),
    [this](const boost::system::error_code& erro, std::size_t bytes_transferred) -> void {
      WriteToSocketCb(erro, bytes_transferred);
    }
  );
}

void Session::WriteToSocketCb(const boost::system::error_code &errc, std::size_t bytes_transferred) {
  if (errc) {
    std::cout << std::format("error msg is: {}\n", errc.message());
  }

  auto &send_data = _queue.front();
  send_data->_curLen += bytes_transferred;

  if (send_data->_curLen < send_data->_totalLen) {
    _sock->async_write_some(
        boost::asio::buffer(send_data->_msg + send_data->_curLen, send_data->_totalLen - send_data->_curLen),
        [this](const boost::system::error_code &errc, std::size_t bytes_transferred) -> void {
          WriteToSocketCb(errc, bytes_transferred);
        });
    return;
  }

  _queue.pop();

  if(_queue.empty()) {
    sending.store(false, std::memory_order_acquire);
  }

  // 发送剩余的数据
  auto &send_util = _queue.front();
  _sock->async_write_some(
      boost::asio::buffer(send_util->_msg, send_util->_totalLen),
      [this](const boost::system::error_code &errc, std::size_t bytes_transferred) -> void {
        WriteToSocketCb(errc, bytes_transferred);
      });
}

void Session::WriteAllToSocket(const std::string &str) {
  _queue.emplace(new MsgNode(str.c_str(), str.length()));
  if(sending) {
    return;
  }

  sending.store(true, std::memory_order_acquire);

  boost::asio::async_write(*_sock, boost::asio::buffer(str),
    [this](const boost::system::error_code& erro, std::size_t bytes_transferred) -> void {
      WriteAllToSocketCb(erro, bytes_transferred);
    }
  );

}


void Session::WriteAllToSocketCb(const boost::system::error_code& errc, std::size_t bytes_transferred) {
  if (errc) {
    std::cout << std::format("error msg is: {}, all bytes is: {}\n", errc.message(), bytes_transferred);
  }

  _queue.pop();

  if(_queue.empty()) {
    sending.store(false, std::memory_order_acquire);
  }

  auto new_send = _queue.front();
  boost::asio::async_write(*_sock, boost::asio::buffer(new_send->_msg, new_send->_totalLen),
    [this](const boost::system::error_code& erro, std::size_t bytes_transferred) -> void {
      WriteAllToSocketCb(erro, bytes_transferred);
    }
  );
}


void Session::ReadFromSocket(){
  _recv_node = std::make_shared<MsgNode>(1024); // 假设每次读取1024字节
  if (receiving) {
    return;
  }

  receiving.store(true, std::memory_order_acquire);

  _sock->async_read_some(boost::asio::buffer(_recv_node->_msg, _recv_node->_totalLen),
    [this](const boost::system::error_code& errc, std::size_t bytes_transferred) -> void {
      ReadFromSocketCb(errc, bytes_transferred);
    }
  );
}

void Session::ReadFromSocketCb(const boost::system::error_code& errc, std::size_t bytes_transferred) {
  if (errc) {
    std::cout << std::format("error msg is: {}\n", errc.message());
  }

  _recv_node->_curLen += bytes_transferred;

  if (_recv_node->_curLen < _recv_node->_totalLen) {
    _sock->async_read_some(
        boost::asio::buffer(_recv_node->_msg + _recv_node->_curLen, _recv_node->_totalLen - _recv_node->_curLen),
        [this](const boost::system::error_code& errc, std::size_t bytes_transferred) -> void {
          ReadFromSocketCb(errc, bytes_transferred);
        });
    return;
  }

  receiving.store(false, std::memory_order_release);
}

void Session::ReadAllFromSocket(){
  _recv_node = std::make_shared<MsgNode>(1024); // 假设每次读取1024字节
  if (receiving) {
    return;
  }

  receiving.store(true, std::memory_order_acquire);

  boost::asio::async_read(*_sock, boost::asio::buffer(_recv_node->_msg, _recv_node->_totalLen),
    [this](const boost::system::error_code& errc, std::size_t bytes_transferred) -> void {
      ReadAllFromSocketCb(errc, bytes_transferred);
    }
  );
}

void Session::ReadAllFromSocketCb(const boost::system::error_code& errc, std::size_t bytes_transferred) {
  receiving.store(false, std::memory_order_release);
  _recv_node = nullptr; // 清理接收节点
}

} // namespace core