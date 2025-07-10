#include <array>
#include <boost/asio.hpp>
#include <boost/asio/impl/read.hpp>
#include <cstddef>
#include <cstring>
#include <format>
#include <iostream>

#define MAX_LENGTH 1024 * 2
#define HEAD_LENGTH 2

int main() {
  try {
    boost::asio::io_context ioc;
    boost::asio::ip::tcp::endpoint enp{boost::asio::ip::make_address_v4("127.0.0.1"), 10088};
    boost::asio::ip::tcp::socket sock{ioc, enp.protocol()};
    sock.connect(enp);

    std::cout << "Please enter msg: ";
    std::array<char, MAX_LENGTH> send_message;
    std::cin.getline(send_message.data(), MAX_LENGTH);
    short len = (short)strlen(send_message.data());
    std::array<char, MAX_LENGTH> trueSend;
    memcpy(trueSend.data(), &len, 2);
    memcpy(trueSend.data() + 2, send_message.data(), (size_t)len);
    boost::asio::write(sock, boost::asio::buffer(trueSend.data(), (size_t)(len + 2)));

    std::array<char, HEAD_LENGTH> reply_head;
    boost::asio::read(sock, boost::asio::buffer(reply_head.data(), HEAD_LENGTH));
    short msgLen = 0;
    memcpy(&msgLen, reply_head.data(), HEAD_LENGTH);
    std::array<char, MAX_LENGTH> msg;
    long long rec_len = (long long)boost::asio::read(sock, boost::asio::buffer(msg.data(), static_cast<size_t>(msgLen)));

    std::cout << "Reply is: ";
    std::cout.write(msg.data(), rec_len) << '\n';
    std::cout << "Reply len is: " << rec_len << '\n';
  } catch (const boost::system::error_code& erro) {
    std::cout << std::format("error code is: {}\n", erro.value());
  }
}