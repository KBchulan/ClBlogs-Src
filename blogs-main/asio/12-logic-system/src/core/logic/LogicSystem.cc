#include "LogicSystem.hpp"

#include <map>
#include <mutex>
#include <queue>
#include <memory>
#include <atomic>
#include <thread>
#include <sstream>
#include <iostream>
#include <condition_variable>

#include <json/json.h>
#include <json/value.h>
#include <json/writer.h>
#include <json/reader.h>

#include <global/Global.hpp>
#include <middleware/Logger.hpp>
#include <core/session/Session.hpp>
#include <core/logic/LogicNode.hpp>
#include <core/msg-node/MsgNode.hpp>

namespace core {

struct LogicSystem::_impl {
  std::queue<std::shared_ptr<LogicNode>> _msg_queue;

  std::jthread _worker_thread;

  std::mutex _queue_mutex;
  std::condition_variable _queue_cv;

  std::atomic_bool _b_stop{false}; // 表示逻辑层是否停止工作
  std::map<short, FunCallBack> _msg_handlers;

  // 注册所有的回调函数
  void RegisterCallback();

  // 处理消息
  void ProcessMessage(const std::shared_ptr<LogicNode>& logic_node);

  _impl() {
    // 注册回调函数
    RegisterCallback();

    // 启动工作线程
    _worker_thread = std::jthread([this]() -> void {
      while (true) {
        std::shared_ptr<LogicNode> logic_node;

        {
          std::unique_lock<std::mutex> lock{_queue_mutex};

          _queue_cv.wait(lock, [this]() -> bool {
            return !_msg_queue.empty() || _b_stop.load(std::memory_order_acquire);
          });

          if (_b_stop.load(std::memory_order_acquire)) {
            break;
          }

          if (!_msg_queue.empty()) {
            logic_node = std::move(_msg_queue.front());
            _msg_queue.pop();
          }
        }

        ProcessMessage(logic_node);
      }

      // 如果停止了，处理剩余的消息
      while (true) {
        std::shared_ptr<LogicNode> logic_node;
        {
          std::scoped_lock<std::mutex> lock{_queue_mutex};
          if (_msg_queue.empty()){
            break;
          }
          logic_node = std::move(_msg_queue.front());
          _msg_queue.pop();
        }
        ProcessMessage(logic_node);
      }

    });
  }

};

void LogicSystem::_impl::RegisterCallback() {
  _msg_handlers[static_cast<short>(MsgType::MSG_HELLO_WORLD)] =
    [](const std::shared_ptr<Session> &session, short msg_id, const char* data) -> void {
      // 读数据
      Json::CharReaderBuilder read_builder;
      std::stringstream strs{data};
      Json::Value recv_data;
      std::string errors;

      if (Json::parseFromStream(read_builder, strs, &recv_data, &errors)) {
        std::cout << std::format("recv test is: {}, recv data is: {}\n", recv_data["test"].asString(), recv_data["data"].asString());
      } else {
        logger.error("Failed to parse JSON data: {}", errors);
        return;
      }

      recv_data["data"] = "server has received msg, " + recv_data["data"].asString();
      Json::StreamWriterBuilder write_builder;
      std::string send_str = Json::writeString(write_builder, recv_data);
      session->Send(msg_id, static_cast<short>(send_str.size()), send_str.c_str());
    };
}

void LogicSystem::_impl::ProcessMessage(const std::shared_ptr<LogicNode>& logic_node) {
  auto msg_id = logic_node->_recvNode->getMsgId();
  auto iter = _msg_handlers.find(msg_id);

  if (iter != _msg_handlers.end()) {
    iter->second(logic_node->_session, msg_id, logic_node->_recvNode->_data);
  } else {
    logger.error("no handler for msg id: {}", msg_id);
  }
}

LogicSystem::LogicSystem() : _pimpl(std::make_unique<_impl>()) {}

LogicSystem::~LogicSystem() {
  _pimpl->_b_stop.store(true, std::memory_order_release);
  _pimpl->_queue_cv.notify_one();
}

void LogicSystem::PostMsgToLogicQueue(const std::shared_ptr<LogicNode> &logic_node) {
  std::unique_lock<std::mutex> lock{_pimpl->_queue_mutex};
  _pimpl->_msg_queue.push(logic_node);

  if (_pimpl->_msg_queue.size() == 1) {
    _pimpl->_queue_cv.notify_one();
  }
}

} // namespace core