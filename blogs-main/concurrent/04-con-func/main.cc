#include <future>
#include <print>
#include <thread>

void func1() {
  using namespace std::chrono_literals;

  auto fetchFromDB = [](const std::string& str) -> std::string {
    std::this_thread::sleep_for(2s);
    return "data from DB: " + str;
  };

  std::future<std::string> res = std::async(std::launch::async, fetchFromDB, "Hello");

  std::print("Current {} is running\n", __FUNCTION__);
  std::print("The task result is: {}\n", res.get());
}

void func2() {
  using namespace std::chrono_literals;

  std::packaged_task<int(int, int)> task{[](int a, int b) -> int {
    std::this_thread::sleep_for(2s);
    return a + b;
  }};

  std::future<int> res = task.get_future();

  std::thread{std::move(task), 1, 2}.detach();

  std::print("Current {} is running\n", __FUNCTION__);
  std::print("The task result is: {}\n", res.get());
}

void func3() {
  using namespace std::chrono_literals;

  std::promise<std::string> prom;

  auto future = prom.get_future();

  std::thread{[&prom]() -> void {
    std::this_thread::sleep_for(2s);
    prom.set_value("Hello from promise");
  }}.detach();

  std::print("Current {} is running\n", __FUNCTION__);
  std::print("The promise result is: {}\n", future.get());
}

int main() {
  func3();
}