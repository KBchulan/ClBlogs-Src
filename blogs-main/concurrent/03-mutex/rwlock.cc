#include <print>
#include <shared_mutex>
#include <unordered_map>
#include <thread>


void func1() {
  std::shared_mutex shared_mutex;
  std::unordered_map<std::string, std::string> shared_entries;

  auto write = [&](const std::string& key, const std::string& value) -> void {
    shared_mutex.lock();

    shared_entries[key] = value;
    std::print("write shared_entries, [{} is {}] ", key, value);

    shared_mutex.unlock();
  };

  auto read = [&](const std::string& key) -> void {
    shared_mutex.lock_shared();
    std::print("read shared_entries, [{} is {}]", key, shared_entries[key]);
    shared_mutex.unlock_shared();
  };

  shared_entries["key1"] = "value1";
  shared_entries["key2"] = "value2";
  shared_entries["key3"] = "value3";

  std::jthread thr_1{read, "key1"};
  std::jthread thr_2{read, "key2"};
  std::jthread thr_3{write, "key4", "value4"};
}

int main() {
  try {
    func1();
  } catch (const std::exception &e) {
    std::print("Exception: {}\n", e.what());
  } catch (...) {
    std::print("Unknown exception occurred\n");
  }
}