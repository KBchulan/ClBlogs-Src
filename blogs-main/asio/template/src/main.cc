#include <boost/asio.hpp>
#include <iostream>

int main() {
  try {
    boost::asio::io_context io_context;
    boost::asio::ip::tcp::endpoint enp{
        boost::asio::ip::make_address_v4("127.0.0.1"), 8080};

    boost::asio::ip::tcp::socket socket(io_context);
    socket.connect(enp);
    std::cout << "Connected to " << enp.address().to_string() << ":"
              << enp.port() << '\n';

  } catch (boost::system::system_error& e) {
    std::cerr << "Error: " << e.what() << '\n';
    return 1;
  }
}
