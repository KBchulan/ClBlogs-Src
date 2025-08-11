#include <print>
#include <processthreadsapi.h>
#include <thread>

void func1_err() {
  // 创建线程1
  std::thread thr1([]() -> void {});

  // 将该线程移交给线程2进行管理，此时 thr1 就空闲了
  std::thread thr2 = std::move(thr1);

  // 重新给 thr1 分配一个新线程
  thr1 = std::thread([]() -> void {});

  // 把线程2给线程1
  thr1 = std::move(thr2);
  thr1.join();
}

void func1() {
  std::thread thr1([]() -> void {});
  std::thread thr2 = std::move(thr1);
  thr1 = std::thread([]() -> void {});

  thr1.detach();
  thr1 = std::move(thr2);
  thr1.join();
}


std::thread rvo_use() {
  return std::thread([]() -> void {
    std::print("RVO thread is running\n");
  });
}
std::thread nrvo_use() {
  std::thread thr([]() -> void {
    std::print("NRVO thread is running\n");
  });
  return thr;
}
void func2() {
  auto thr1 = rvo_use();
  thr1.join();

  auto thr2 = nrvo_use();
  thr2.join();
}


void func3() {
  auto id = std::this_thread::get_id();
  std::print("Current thread ID: {}\n", id);
}

void func4() {
  thread_local int thread_local_var = 0;

  std::jthread thr1{[&]() -> void {
    thread_local_var++;
    std::print("Thread 1, thread_local_var: {}\n", thread_local_var);  // 1
  }};

  std::print("Main Thread, thread_local_var: {}\n", thread_local_var); // 0
}

int main() {
  try {
    func4();
  } catch (const std::exception &e) {
    std::print("Exception: {}\n", e.what());
  } catch (...) {
    std::print("Unknown exception occurred\n");
  }
}