/******************************************************************************
 *
 * @file       MsgNode.hpp
 * @brief      消息节点的统一定义
 *
 * @author     KBchulan
 * @date       2025/07/28
 * @history
 ******************************************************************************/

#ifndef MSGNODE_HPP
#define MSGNODE_HPP

#include <core/CoreExport.hpp>

namespace core {

class CORE_EXPORT MsgNode {
public:
  MsgNode(short msg_len);

  virtual ~MsgNode();

  void Clear();

  [[nodiscard]] virtual short getMsgId() const;

  MsgNode(const MsgNode &) = delete;
  MsgNode(MsgNode &&) = delete;
  MsgNode &operator=(const MsgNode &) = delete;
  MsgNode &operator=(MsgNode &&) = delete;

  short _cur_len{};
  short _msg_len;
  char *_data;
};

class CORE_EXPORT RecvNode final : public MsgNode {
public:
  RecvNode(short msg_id, short msg_len);

  [[nodiscard]] short getMsgId() const override;

private:
  short _msg_id;
};

class CORE_EXPORT SendNode final : public MsgNode {
public:
  SendNode(short msg_id, short msg_len, const char *data);

  [[nodiscard]] short getMsgId() const override;

private:
  short _msg_id;
};

} // namespace core


#endif // MSGNODE_HPP