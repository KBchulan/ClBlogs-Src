#ifndef MSGNODE_HPP
#define MSGNODE_HPP

#include <cstring>
#include <cstddef>

#include <core/CoreExport.hpp>


namespace core {

class CORE_EXPORT MsgNode {
  friend class Session;
public:
  MsgNode(char *data, std::size_t max_len) : _max_len(max_len) {
    _data = new char[max_len];
    memcpy(_data, data, max_len);
  }

  ~MsgNode() {
    delete []_data;
  }

private:
  std::size_t _cur_len{};
  std::size_t _max_len;
  char* _data;
};

} // namespace core


#endif // MSGNODE_HPP