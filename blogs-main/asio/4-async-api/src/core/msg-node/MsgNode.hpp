#ifndef MSGNODE_HPP
#define MSGNODE_HPP

#include <core/CoreExport.hpp>
#include <cstddef>
#include <cstring>
#include <iostream>

namespace core {

class CORE_EXPORT MsgNode {
public:
  // 发送节点时
  MsgNode(const char *msg, std::size_t total) : _totalLen(total), _curLen(0) {
    _msg = new char[total];
    memcpy(_msg, msg, total);
    std::cout << std::format("total length is: {}, cur length is: {}\n", _totalLen, _curLen);
  }

  // 做读的节点时
  MsgNode(std::size_t total) : _totalLen(total), _curLen(0) {
    _msg = new char[total];
    std::cout << std::format("total length is: {}, cur length is: {}\n", _totalLen, _curLen);
  }

  ~MsgNode() { delete[] _msg; }

  std::size_t _totalLen;
  std::size_t _curLen;
  char *_msg;
};

} // namespace core

#endif // MSGNODE_HPP