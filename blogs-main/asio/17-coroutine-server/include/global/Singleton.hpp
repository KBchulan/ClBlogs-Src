/******************************************************************************
 *
 * @file       Singleton.hpp
 * @brief      单例模板的实现
 *
 * @author     KBchulan
 * @date       2025/04/03
 * @history
 ******************************************************************************/

#ifndef SINGLETON_HPP
#define SINGLETON_HPP

namespace global {

template <typename T>
class Singleton {
 public:
  static T &getInstance() noexcept {
    static T instance;
    return instance;
  }

  Singleton<T>(const Singleton<T> &) = delete;
  Singleton<T> &operator=(const Singleton<T> &) = delete;

  Singleton<T>(Singleton<T> &&) = delete;
  Singleton<T> &operator=(Singleton<T> &&) = delete;

 protected:
  Singleton() = default;
  ~Singleton() = default;
};

}  // namespace global

#endif  // SINGLETON_HPP