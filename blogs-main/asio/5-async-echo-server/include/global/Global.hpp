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

namespace global {

class GlobalVariable final : public Singleton<GlobalVariable> {};

} // namespace global

#define MAX_LENGTH 1024
#define globalVariable global::GlobalVariable::getInstance()

#endif // GLOBAL_HPP
