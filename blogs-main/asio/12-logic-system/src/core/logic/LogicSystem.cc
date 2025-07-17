#include "LogicSystem.hpp"

#include <core/session/Session.hpp>

namespace core {

struct LogicSystem::_impl {

};

LogicSystem::LogicSystem() : _pimpl(std::make_unique<_impl>()) {}

LogicSystem::~LogicSystem() = default;

} // namespace core