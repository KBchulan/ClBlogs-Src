#include "boost/asio/io_context.hpp"
#include "boost/asio/ip/address.hpp"
#include "boost/asio/ip/address_v4.hpp"
#include "boost/asio/ip/tcp.hpp"
#include "boost/system/detail/error_code.hpp"
#include <format>
#include <iostream>
#include <string>

void client_endpoint() {
  std::string ip_raw = std::string("127.0.0.1");
  unsigned short port = 3333;

  // 错误处理
  boost::system::error_code erroc;
  boost::asio::ip::address ip = boost::asio::ip::make_address_v4(ip_raw, erroc);

  if (erroc.value() != 0) {
    std::cout << std::format("Failed to construct ip, error value is: {}, error msg is: {}\n",
      erroc.value(), erroc.message());
  }

  boost::asio::ip::tcp::endpoint ep{ip, port};
}

void server_endpoint() {
  unsigned short port = 3333;
  boost::asio::ip::address add{boost::asio::ip::address_v4::any()};
  boost::asio::ip::tcp::endpoint ep{add, port};
}

void create_tcp_socket() {
  boost::asio::io_context ioc;
  boost::asio::ip::tcp protocol{boost::asio::ip::tcp::v4()};
  boost::asio::ip::tcp::socket socket{ioc, protocol};

  try {
    socket.open(protocol);
  } catch (const boost::system::error_code &ec) {
    std::cout << std::format("Failed to construct ip, error value is: {}, error msg is: {}\n",
        ec.value(), ec.message());
  }
}

void create_appertor_socket() {
  boost::asio::io_context ioc;
  boost::asio::ip::tcp protocol{boost::asio::ip::tcp::v4()};
  boost::asio::ip::tcp::acceptor acceptor(ioc, protocol);

  try {
    acceptor.open(protocol);
  }  catch (const boost::system::error_code &ec) {
    std::cout << std::format("Failed to construct ip, error value is: {}, error msg is: {}\n",
        ec.value(), ec.message());
  }
}

void create_bind_socket() {
  unsigned short port = 3333;
  boost::asio::io_context ioc;
  boost::asio::ip::tcp::endpoint ep{boost::asio::ip::address_v4::any(), port};

  // 这种创建出来的是没有绑定端口的acceptor，需要我们手动绑定
  boost::asio::ip::tcp::acceptor acce{ioc, ep.protocol()};

  try {
    acce.bind(ep);
  } catch (const boost::system::error_code &ec) {
    std::cout << std::format("Failed to construct ip, error value is: {}, error msg is: {}\n",
        ec.value(), ec.message());
  }
}

void connect_to_endpoint() {
  std::string raw_add{"127.0.0.1"};
  unsigned short port = 3333;

  try {
    boost::asio::io_context ioc;
    boost::asio::ip::tcp::endpoint ep{boost::asio::ip::make_address_v4(raw_add), port};

    boost::asio::ip::tcp::socket sock{ioc, ep.protocol()};

    sock.connect(ep);

  } catch (const boost::system::error_code &ec) {
    std::cout << std::format("Failed to construct ip, error value is: {}, error msg is: {}\n",
        ec.value(), ec.message());
  }
}

void dns_connect_to_endpoint() {
  std::string host = "chulan.xin";
  std::string port = "80";

  try {
    boost::asio::io_context ioc;
    boost::asio::ip::tcp::resolver resolver{ioc};
    auto endpoints = resolver.resolve(host, port);

    std::cout << std::format("{} {}", endpoints.begin()->endpoint().address().to_string(), endpoints.begin()->endpoint().port());

    // 连接解析出来的第一个东西
    boost::asio::ip::tcp::socket sock{ioc};
    sock.connect(*endpoints.begin());

  } catch (const boost::system::error_code &ec) {
    std::cout << std::format("Failed to construct ip, error value is: {}, error msg is: {}\n",
        ec.value(), ec.message());
  }
}

void accept_new_connection() {
  unsigned short port = 3333;
  int BLOCK_SIZE = 30;  // 指定30个最大的
  boost::asio::ip::tcp::endpoint ep{boost::asio::ip::address_v4::any(), port};
  boost::asio::io_context ioc;

  try {
    boost::asio::ip::tcp::acceptor acceptor{ioc, ep.protocol()};
    acceptor.bind(ep);

    // 可以指定最多监听多少
    acceptor.listen(BLOCK_SIZE);
    // 开一个socket用于通信
    boost::asio::ip::tcp::socket sock{ioc, ep.protocol()};
    acceptor.accept(sock);
  } catch (const boost::system::error_code &ec) {
    std::cout << std::format("Failed to construct ip, error value is: {}, error msg is: {}\n",
        ec.value(), ec.message());
  }
}

int main() {
  // create_tcp_socket();
  // create_appertor_socket();
  dns_connect_to_endpoint();
}