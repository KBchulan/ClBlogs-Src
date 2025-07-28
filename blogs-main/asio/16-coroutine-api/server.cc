#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/this_coro.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/write.hpp>
#include <boost/system/system_error.hpp>

#include <iostream>
#include <array>

boost::asio::awaitable<void> echo(boost::asio::ip::tcp::socket soc) {
  try {
    std::array<char, 1024> data;
    while (true) {
      auto size = co_await soc.async_read_some(boost::asio::buffer(data.data(), 1024), boost::asio::use_awaitable);
      co_await boost::asio::async_write(soc, boost::asio::buffer(data.data(), size), boost::asio::use_awaitable);
    }
  } catch (boost::system::system_error &se) {
    std::cerr << std::format("error code is: {}, error msg is: {}\n", se.code().value(), se.code().message());
  }
}

boost::asio::awaitable<void> acceptor() {
  auto exector = co_await boost::asio::this_coro::executor;
  boost::asio::ip::tcp::acceptor acceptor{exector, {boost::asio::ip::address_v4::any(), 10088}};
  std::cout << "Server is running on port 10088\n";

  while (true) {
    auto socket = co_await acceptor.async_accept(boost::asio::use_awaitable);
    boost::asio::co_spawn(exector, echo(std::move(socket)), boost::asio::detached);
  }
}

int main() {
  try {
    boost::asio::io_context ioc;
    boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
    signals.async_wait([&ioc](const boost::system::error_code &ec, int signal_number) -> void {
      if (!ec) {
        std::cout << "Received signal: " << signal_number << ". Stopping io_context.\n";
        ioc.stop();
      } else {
        std::cerr << "Error waiting for signal: " << ec.message() << '\n';
      }
    });

    boost::asio::co_spawn(ioc, acceptor(), boost::asio::detached);

    ioc.run();
  } catch (boost::system::system_error &se) {
    std::cerr << std::format("error code is: {}, error msg is: {}\n", se.code().value(), se.code().message());
  }
}