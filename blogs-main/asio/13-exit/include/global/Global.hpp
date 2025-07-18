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

#include <cstdint>

#define MSG_TYPE_LENGTH 2
#define MSG_LEN_LENGTH 2
#define MSG_HEAD_TOTAL_LEN 4
#define MSG_BODY_LENGTH 1024 * 2
#define SEND_QUEUE_MAX_LEN 1000

enum class MsgType : std::uint16_t {
  MSG_HELLO_WORLD = 1001,
};

#endif // GLOBAL_HPP
