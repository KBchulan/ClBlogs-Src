#ifndef LOGICSYSTEM_HPP
#define LOGICSYSTEM_HPP

#include <memory>
#include <functional>

#include <core/CoreExport.hpp>
#include <global/Singleton.hpp>

namespace core {

class Session;
class LogicNode;
class CORE_EXPORT LogicSystem final : public global::Singleton<LogicSystem> {
  friend class global::Singleton<LogicSystem>;

  /**
    * @brief 回调函数类型，用于处理接收到的消息
    * @param session 共享指针，指向当前会话
    * @param msg_id 消息ID
    * @param data 消息数据
    **/
  using FunCallBack = std::function<void(const std::shared_ptr<Session>&, short, const char*)>;

private:
  LogicSystem();

public:
  ~LogicSystem();

  void PostMsgToLogicQueue(const std::shared_ptr<LogicNode> &logic_node);

private:
  struct _impl;
  std::unique_ptr<_impl> _pimpl;
};

} // namespace core

#define logicSystem core::LogicSystem::getInstance()

#endif // LOGICSYSTEM_HPP