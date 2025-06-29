#include "boost/asio/ip/tcp.hpp"
#include <boost/asio.hpp>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <iostream>

void send_mock_buffer() {
  std::string str{"hello, buffer"};
  boost::asio::const_buffer cb{str.c_str(), str.length()};

  std::vector<boost::asio::const_buffer> const_buffer_sq;
  const_buffer_sq.emplace_back(cb);

  boost::asio::io_context ioc;
  boost::asio::ip::tcp::socket sock{ioc};

  sock.send(const_buffer_sq);
}

void use_buffer_1() {
  boost::asio::const_buffer res = boost::asio::buffer("hello, buffer");
}

void use_buffer_2() {
  constexpr std::uint16_t BLOCK_SIZE = 20;
  std::unique_ptr<char> buf{new char[BLOCK_SIZE]};
  auto res= boost::asio::buffer(reinterpret_cast<void*>(buf.get()), BLOCK_SIZE);
}

void write_to_socket(boost::asio::ip::tcp::socket& sock) {
  std::string str{"hello, write some"};
  std::size_t total_length = 0;

  while (total_length != str.length()) {
    total_length += sock.write_some(boost::asio::buffer(str.c_str() + total_length, str.length() - total_length));
  }
}

void demonstrate_write_at() {
  boost::asio::io_context ioc;

  try {
    boost::asio::random_access_file file(
        ioc, "test.txt",
        boost::asio::random_access_file::write_only |
            boost::asio::random_access_file::create);

    std::string data1 = "Hello ";
    std::string data2 = "World!";

    boost::asio::write_at(file, 0, boost::asio::buffer(data1));

    boost::asio::write_at(file, 6, boost::asio::buffer(data2));
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
}

void read_to_socket(boost::asio::ip::tcp::socket& sock) {
  constexpr std::uint16_t BLOCK_SIZE = 20;
  char buffer[BLOCK_SIZE] = {};
  std::size_t total_length = 0;

  while (total_length != BLOCK_SIZE) {
    total_length += sock.read_some(boost::asio::buffer(buffer + total_length, BLOCK_SIZE - total_length));
  }
}