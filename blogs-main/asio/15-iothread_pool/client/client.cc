#include <boost/asio.hpp>
#include <boost/asio/ip/address.hpp>

#include <json/json.h>
#include <json/reader.h>
#include <json/value.h>
#include <json/writer.h>

#include <chrono>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#define HEAD_LENGTH 2
#define MAX_LENGTH 1024 * 2

void clientThread(int threadId) {
  try {
    boost::asio::io_context ioc;
    boost::asio::ip::tcp::endpoint enp{boost::asio::ip::make_address_v4("127.0.0.1"), 10088};
    boost::asio::ip::tcp::socket sock{ioc};
    sock.connect(enp);

    for (int i = 0; i < 500; ++i) {
      // 发送数据
      Json::Value root;
      root["test"] = "test str";
      root["data"] = "hello from thread " + std::to_string(threadId) + " message " + std::to_string(i);
      Json::StreamWriterBuilder builder;
      std::string raw_str = Json::writeString(builder, root);

      short data_id = 1001;
      data_id = (short)boost::asio::detail::socket_ops::host_to_network_short(static_cast<u_short>(data_id));
      auto send_len = static_cast<short>(raw_str.length());
      send_len = (short)boost::asio::detail::socket_ops::host_to_network_short(static_cast<u_short>(send_len));
      std::array<char, MAX_LENGTH> send_date;
      memcpy(send_date.data(), &data_id, HEAD_LENGTH);
      memcpy(send_date.data() + HEAD_LENGTH, &send_len, HEAD_LENGTH);
      memcpy(send_date.data() + HEAD_LENGTH + HEAD_LENGTH, raw_str.c_str(), raw_str.size());
      boost::asio::write(sock, boost::asio::buffer(send_date.data(), raw_str.length() + HEAD_LENGTH + HEAD_LENGTH));

      // 接收数据
      std::array<char, HEAD_LENGTH> head_id;
      boost::asio::read(sock, boost::asio::buffer(head_id.data(), HEAD_LENGTH));
      short head_id_local = 0;
      memcpy(&head_id_local, head_id.data(), HEAD_LENGTH);
      head_id_local = (short)boost::asio::detail::socket_ops::network_to_host_short(static_cast<u_short>(head_id_local));

      // 读取头部长度
      std::array<char, HEAD_LENGTH> head_len;
      boost::asio::read(sock, boost::asio::buffer(head_len.data(), HEAD_LENGTH));
      short len = 0;
      memcpy(&len, head_len.data(), HEAD_LENGTH);
      len = (short)boost::asio::detail::socket_ops::network_to_host_short(static_cast<u_short>(len));

      std::array<char, MAX_LENGTH> recv;
      boost::asio::read(sock, boost::asio::buffer(recv.data(), static_cast<size_t>(len)));

      Json::CharReaderBuilder read_builder;
      std::stringstream sss{recv.data()};
      Json::Value read;
      std::string errors;
      Json::parseFromStream(read_builder, sss, &read, &errors);

      if (i % 100 == 0) {  // 每100次打印一次进度
        std::cout << "Thread " << threadId << " - Message " << i << " received: " << read["test"].asString() << " | " << read["data"].asString() << '\n';
      }
    }

    std::cout << "Thread " << threadId << " completed 500 messages\n";

  } catch (const std::exception &ex) {
    std::cout << "Thread " << threadId << " error: " << ex.what() << '\n';
  } catch (const boost::system::error_code &err) {
    std::cout << "Thread " << threadId << " error code: " << err.value() << '\n';
  }
}

int main() {
  std::vector<std::thread> threads;
  threads.reserve(100);

  auto start_time = std::chrono::high_resolution_clock::now();
  std::cout << "Starting 100 client threads...\n";

  for (int i = 0; i < 100; ++i) {
    threads.emplace_back(clientThread, i);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::cout << "Created thread " << i << '\n';
  }

  std::cout << "All threads created, waiting for completion...\n";

  // 等待所有线程完成
  for (auto &thread : threads) {
    thread.join();
  }

  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

  std::cout << "All threads completed!\n";
  std::cout << "Total execution time: " << duration.count() << " ms (" << static_cast<double>(duration.count()) / 1000.0 << " seconds)\n";

  return 0;
}