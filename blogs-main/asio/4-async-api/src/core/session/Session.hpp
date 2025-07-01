#ifndef SESSION_HPP
#define SESSION_HPP

#include <atomic>
#include <boost/asio/ip/tcp.hpp>
#include <boost/system/detail/error_code.hpp>

#include <core/CoreExport.hpp>
#include <core/msg-node/MsgNode.hpp>
#include <cstddef>
#include <memory>
#include <queue>

namespace core {

class CORE_EXPORT Session {
public:
  Session(std::shared_ptr<boost::asio::ip::tcp::socket> sock);

  void Connect(const boost::asio::ip::tcp::endpoint &enp);

  void WriteToSocketErr(const std::string &str);
  void WriteToSocketCbErr(const boost::system::error_code& errc, std::size_t bytes_transferred);

  void WriteToSocket(const std::string &str);
  void WriteToSocketCb(const boost::system::error_code& errc, std::size_t bytes_transferred);

  void WriteAllToSocket(const std::string &str);
  void WriteAllToSocketCb(const boost::system::error_code& errc, std::size_t bytes_transferred);

  void ReadFromSocket();
  void ReadFromSocketCb(const boost::system::error_code& errc, std::size_t bytes_transferred);

  void ReadAllFromSocket();
  void ReadAllFromSocketCb(const boost::system::error_code& errc, std::size_t bytes_transferred);

private:
  std::shared_ptr<boost::asio::ip::tcp::socket> _sock;
  std::shared_ptr<MsgNode> _send_node;

  std::queue<std::shared_ptr<MsgNode>> _queue;
  std::atomic_bool sending{false};

  std::shared_ptr<MsgNode> _recv_node;
  std::atomic_bool receiving{false};
};

}; // namespace core

#endif // SESSION_HPP