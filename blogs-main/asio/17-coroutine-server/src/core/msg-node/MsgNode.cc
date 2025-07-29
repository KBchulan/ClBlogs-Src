#include "MsgNode.hpp"

#include <cstddef>
#include <cstring>
#include <winsock2.h>

#include <global/Global.hpp>
#include <boost/asio/detail/socket_holder.hpp>

namespace core {

MsgNode::MsgNode(short msg_len) : _msg_len(msg_len) {
  _data = new char[static_cast<std::size_t>(_msg_len + 1)]();
  _data[_msg_len] = '\0';
}

MsgNode::~MsgNode() {
  delete[] _data;
}

void MsgNode::Clear() {
  memset(_data, 0, static_cast<size_t>(_msg_len));
  _cur_len = 0;
}

short MsgNode::getMsgId() const {
  return -1;
}


RecvNode::RecvNode(short msg_id, short msg_len)
  : MsgNode(msg_len), _msg_id(msg_id) {}

short RecvNode::getMsgId() const {
  return this->_msg_id;
}


SendNode::SendNode(short msg_id, short msg_len, const char *data)
  : MsgNode(msg_len + MSG_HEAD_TOTAL_LEN), _msg_id(msg_id) {
  auto net_msg_id = (short)boost::asio::detail::socket_ops::host_to_network_short(static_cast<u_short>(msg_id));
  memcpy(_data, &net_msg_id, MSG_TYPE_LENGTH);
  auto net_msg_len = (short)boost::asio::detail::socket_ops::host_to_network_short(static_cast<u_short>(msg_len));
  memcpy(_data + MSG_TYPE_LENGTH, &net_msg_len, MSG_LEN_LENGTH);
  memcpy(_data + MSG_HEAD_TOTAL_LEN, data, static_cast<size_t>(msg_len));
}

short SendNode::getMsgId() const {
  return this->_msg_id;
}

} // namespace core