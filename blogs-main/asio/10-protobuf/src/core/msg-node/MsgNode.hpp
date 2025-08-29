#ifndef MSGNODE_HPP
#define MSGNODE_HPP

#include <cstddef>
#include <cstring>

#include <global/Global.hpp>
#include <core/CoreExport.hpp>

#include <boost/asio/detail/socket_ops.hpp>

namespace core {

class CORE_EXPORT MsgNode {
  friend class Session;
public:
  // 发送节点的构造
  MsgNode(char *data, short max_len) : _max_len(max_len + HEAD_LENGTH) {
    _data = new char[static_cast<size_t>(_max_len + 1)]();
    auto head_len = boost::asio::detail::socket_ops::host_to_network_short(static_cast<u_short>(max_len));
    memcpy(_data, &head_len, HEAD_LENGTH);
    memcpy(_data + HEAD_LENGTH, data, static_cast<size_t>(max_len));
    _data[_max_len] = '\0';
  }

  // 接收节点的构造
  MsgNode(short max_len) : _max_len(max_len) {
    _data = new char[static_cast<size_t>(_max_len + 1)]();
  }

  ~MsgNode() {
    delete []_data;
  }

private:
  short _cur_len{};
  short _max_len;
  char* _data;
};

} // namespace core


#endif // MSGNODE_HPP