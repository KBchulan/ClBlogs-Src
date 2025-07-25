#ifndef SESSION_HPP
#define SESSION_HPP

#include <array>
#include <atomic>
#include <mutex>
#include <queue>
#include <memory>
#include <cstddef>

#include <boost/asio/ip/tcp.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/system/detail/error_code.hpp>

#include <core/CoreExport.hpp>
#include <global/Global.hpp>

namespace core {

class Server;
class MsgNode;
class CORE_EXPORT Session : public std::enable_shared_from_this<Session> {
public:
  Session(boost::asio::io_context& ioc, Server* server) : _sock(ioc), _server(server) {
    boost::uuids::uuid uuid = boost::uuids::random_generator_mt19937()();
    _uuid = boost::uuids::to_string(uuid);
  }

  Session(const Session&) = delete;
  Session& operator=(const Session&) = delete;

  void Start();
  void Close();
  void Send(short msg_id, short msg_len, const char *data);

  boost::asio::ip::tcp::socket& getSocket() { return _sock; }
  std::string getUUid() { return _uuid; }

private:
  void handle_read(const boost::system::error_code& err, std::size_t bytes_transferred);
  void handle_write(const boost::system::error_code& err);

  boost::asio::ip::tcp::socket _sock;
  std::array<char, MSG_BODY_LENGTH> _data;

  // 服务器管理会话使用
  Server *_server;
  std::string _uuid;

  // 发送队列
  std::queue<std::shared_ptr<MsgNode>> _send_queue;
  std::mutex _send_mtx;

  // 接收节点的处理
  std::shared_ptr<MsgNode> _recv_head_node;
  std::shared_ptr<MsgNode> _recv_msg_node;
  std::atomic_bool _head_parse;
};

} // namespace core


#endif // SESSION_HPP