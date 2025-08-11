#include <cstddef>
#include <map>
#include <mutex>
#include <print>
#include <string>

class error_use {
public:
  void increment() {
    mtx.lock();
    ++count;
    std::print("increment: count = {}\n", count);
    mtx.unlock();
  }

  void decrement() {
    mtx.lock();
    --count;
    std::print("decrement: count = {}\n", count);
    mtx.unlock();
  }

  void callAll() {
    mtx.lock();
    increment(); // 触发死锁
    decrement();
    mtx.unlock();
  }

  size_t get_count() const {
    mtx.lock();
    size_t current_count = count;
    mtx.unlock();
    return current_count;
  }

private:
  size_t count = 50;
  mutable std::mutex mtx;
};

class FileSystem {
public:
  void createFile(const std::string &filename, const std::string &content) {
    _rec_mtx.lock();
    _files[filename] = content;
    std::print("File created: [{}] with content: [{}]\n", filename, content);
    _rec_mtx.unlock();
  }

  std::string readFile(const std::string &name) {
    _rec_mtx.lock();
    return _files[name];
    _rec_mtx.unlock();
  }

  // 复制文件
  void copyFile(const std::string &source, const std::string &dest) {
    _rec_mtx.lock();
    std::string content = readFile(source);
    createFile(dest, content);
    std::print("File copied from [{}] to [{}]\n", source, dest);
    _rec_mtx.unlock();
  }

private:
  std::map<std::string, std::string> _files;
  mutable std::recursive_mutex _rec_mtx;
};

int main() {
  try {
    FileSystem fs;
    fs.createFile("file1.txt", "Hello, World!");
    fs.createFile("file2.txt", "This is a test file.");
    fs.copyFile("file1.txt", "file3.txt");
  } catch (const std::exception &e) {
    std::print("Exception: {}\n", e.what());
  } catch (...) {
    std::print("Unknown exception occurred\n");
  }
  return 0;
}