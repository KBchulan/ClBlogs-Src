#ifndef LOGICNODE_HPP
#define LOGICNODE_HPP

#include <memory>

#include <core/CoreExport.hpp>

namespace core {

class Session;
class RecvNode;
class CORE_EXPORT LogicNode {
  friend class LogicSystem;

public:
  LogicNode(std::shared_ptr<Session> session, std::shared_ptr<RecvNode> recvNode)
      : _session(std::move(session)), _recvNode(std::move(recvNode)) {}

private:
  std::shared_ptr<Session> _session;
  std::shared_ptr<RecvNode> _recvNode;
};

} // namespace core

#endif // LOGICNODE_HPP