#include <boost/asio.hpp>
#include <cstddef>
#include <cstring>
#include <iostream>

#define MAX_LENGTH 1024

int main() {
  try {
    boost::asio::io_context ioc;
    boost::asio::ip::tcp::endpoint ep{boost::asio::ip::make_address_v4("127.0.0.1"), 10088};

    boost::asio::ip::tcp::socket sock{ioc, ep.protocol()};

    sock.connect(ep);

    std::cout << "Enter message: ";
    char send[MAX_LENGTH];
    std::cin.getline(send, MAX_LENGTH);
    std::size_t length = strlen(send);
    boost::asio::write(sock, boost::asio::buffer(send, length));

    char receive[MAX_LENGTH];
    std::size_t recive_len = boost::asio::read(sock, boost::asio::buffer(receive, length));
    std::cout << "Received message is: ";
    std::cout.write(receive, recive_len);
    std::cout << '\n';

  } catch (const boost::system::system_error& ex) {
    std::cout << std::format("error code is: {}, msg is: {}\n", ex.code().value(), ex.code().message());
  }
}