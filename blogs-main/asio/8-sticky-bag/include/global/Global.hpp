/******************************************************************************
 *
 * @file       Global.hpp
 * @brief      这里写全局的声明配置
 *
 * @author     KBchulan
 * @date       2025/04/03
 * @history
 ******************************************************************************/

#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include <global/Singleton.hpp>

#include <map>
#include <any>
#include <string>

namespace global {

class GlobalVariable final : public Singleton<GlobalVariable> {
public:
  std::map<std::string, std::any> globalValue;
};

} // namespace global

#define HEAD_LENGTH 2
#define MAX_LENGTH 1024 * 2

#define globalVariable global::GlobalVariable::getInstance()

#endif // GLOBAL_HPP
