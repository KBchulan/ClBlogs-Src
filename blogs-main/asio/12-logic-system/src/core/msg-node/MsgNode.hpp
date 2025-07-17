#ifndef MSGNODE_HPP
#define MSGNODE_HPP

#include <cstddef>
#include <cstring>

#include <global/Global.hpp>
#include <core/CoreExport.hpp>
#include <middleware/Logger.hpp>

#include <boost/asio/detail/socket_holder.hpp>

namespace core {

// 头部节点
class CORE_EXPORT MsgNode {
public:
  MsgNode(short msg_len) : _msg_len(msg_len) {
    _data = new char[static_cast<size_t>(_msg_len + 1)]();
    _data[_msg_len] = '\0';
  }

  virtual ~MsgNode() {
    delete[] _data;
  }

  [[nodiscard]] virtual short getMsgId() const {
    return -1;
  };

  MsgNode(const MsgNode &) = delete;
  MsgNode(MsgNode &&) = delete;
  MsgNode &operator=(const MsgNode &) = delete;
  MsgNode &operator=(MsgNode &&) = delete;

  void Clear() {
    memset(_data, 0, static_cast<size_t>(_msg_len));
    _cur_len = 0;
  }

  short _cur_len{};
  short _msg_len;
  char *_data;
};

class CORE_EXPORT RecvNode final : public MsgNode {
public:
  RecvNode(short msg_id, short msg_len) : MsgNode(msg_len), _msg_id(msg_id) { }

  [[nodiscard]] short getMsgId() const override { return this->_msg_id; }

private:
  short _msg_id;
};

class CORE_EXPORT SendNode final : public MsgNode {
public:
  SendNode(short msg_id, short msg_len, const char *data) : MsgNode(msg_len + MSG_HEAD_TOTAL_LEN), _msg_id(msg_id) {
    auto msg_id_net = (short)boost::asio::detail::socket_ops::host_to_network_short(static_cast<u_short>(msg_id));
    memcpy(_data, &msg_id_net, MSG_TYPE_LENGTH);
    auto msg_len_net = (short)boost::asio::detail::socket_ops::host_to_network_short(static_cast<u_short>(msg_len));
    memcpy(_data + MSG_TYPE_LENGTH, &msg_len_net, MSG_LEN_LENGTH);
    memcpy(_data + MSG_HEAD_TOTAL_LEN, data, static_cast<size_t>(msg_len));
  }

  [[nodiscard]] short getMsgId() const override { return this->_msg_id; }

private:
  short _msg_id;
};

} // namespace core

#endif // MSGNODE_HPP