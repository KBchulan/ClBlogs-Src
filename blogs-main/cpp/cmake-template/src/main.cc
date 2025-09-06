#include <tools/Logger.hpp>

// 如果有单例实现，可以在此处初始化
void init() {
  logger;
}

int main()
{
  std::print("--------- Init Begin ---------\n");
  init();
  std::print("--------- Init End ---------\n\n");

  std::print("--------- Application Begin ---------\n");
  logger.info("Hello, {}", "World");
  std::print("--------- Application End ---------\n");
}