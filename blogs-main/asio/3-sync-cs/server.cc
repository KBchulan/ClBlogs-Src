#include <boost/asio.hpp>
#include <cstring>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

#define MAX_LENGTH 1024

using PtrSocket = std::shared_ptr<boost::asio::ip::tcp::socket>;
std::vector<std::shared_ptr<std::jthread>> threads;

void session(PtrSocket sock) {
  try {
    for (;;) {
      char receive[MAX_LENGTH];
      memset(receive, '\0', MAX_LENGTH);
      boost::system::error_code ec;

      // 此处我们假设不存在粘包情况，即采用read_some也能读完
      sock->read_some(boost::asio::buffer(receive, MAX_LENGTH), ec);

      if (ec == boost::asio::error::eof) {
        std::cout << std::format("connection error: {}\n", ec.message());
        break;
      } else if (ec) {
        throw boost::system::system_error{ec};
      }

      std::cout << std::format("Receive from client: {}, message is: {}\n", sock->remote_endpoint().address().to_string(), receive);

      // 发送回去
      boost::asio::write(*sock, boost::asio::buffer(receive, MAX_LENGTH), ec);
    }
  } catch (const boost::system::system_error &se) {
    std::cout << std::format("error code is: {}, error msg is: {}\n", se.code().value(), se.code().message());
  }
}

void server(boost::asio::io_context &ioc, unsigned short port) {
  boost::asio::ip::tcp::endpoint ep{boost::asio::ip::address_v4::any(), port};
  boost::asio::ip::tcp::acceptor acceptor{ioc, ep};

  std::cout << "Server is running on port " << port << '\n';

  for (;;) {
    PtrSocket sock{new boost::asio::ip::tcp::socket{ioc}};
    acceptor.accept(*sock);

    // 进入通信
    auto thr = std::make_shared<std::jthread>(session, sock);
    threads.emplace_back(thr);
  }
}

int main() {
  try {
    boost::asio::io_context ioc;
    server(ioc, 10088);

  } catch (const boost::system::system_error &se) {
    std::cout << std::format("error code is: {}, error msg is: {}\n", se.code().value(), se.code().message());
  }
}