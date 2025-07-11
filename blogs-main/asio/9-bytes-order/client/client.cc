#include <boost/asio.hpp>
#include <boost/asio/ip/address.hpp>
#include <iostream>
#include <thread>

using namespace std;
using namespace boost::asio::ip;
const int MAX_LENGTH = 1024 * 2;
const int HEAD_LENGTH = 2;

int main() {
  try {
    boost::asio::io_context ioc;
    tcp::endpoint remote_ep(make_address("127.0.0.1"), 10088);
    tcp::socket sock(ioc);
    boost::system::error_code error = boost::asio::error::host_not_found;

    sock.connect(remote_ep, error);
    if (error) {
      cout << "connect failed, code is " << error.value() << " error msg is "
           << error.message();
      return 0;
    }

    thread send_thread([&sock] {
      for (;;) {
        this_thread::sleep_for(std::chrono::milliseconds(2));
        const char *request = "hello world!";
        auto request_length = static_cast<short>(strlen(request));
        char send_data[MAX_LENGTH] = {0};
        // 转为网络字节序
        auto request_host_length = (short)boost::asio::detail::socket_ops::host_to_network_short(static_cast<u_short>(request_length));
        memcpy(send_data, &request_host_length, 2);
        memcpy(send_data + 2, request, static_cast<size_t>(request_length));
        boost::asio::write(sock,boost::asio::buffer(send_data, static_cast<size_t>(request_length + HEAD_LENGTH)));
      }
    });

    thread recv_thread([&sock] {
      for (;;) {
        this_thread::sleep_for(std::chrono::milliseconds(2));
        cout << "begin to receive...\n";
        char reply_head[HEAD_LENGTH];
        boost::asio::read(sock, boost::asio::buffer(reply_head, HEAD_LENGTH));
        short msglen = 0;
        memcpy(&msglen, reply_head, HEAD_LENGTH);
        // 转为本地字节序
        msglen = (short)boost::asio::detail::socket_ops::network_to_host_short(static_cast<u_short>(msglen));
        char msg[MAX_LENGTH] = {0};
        boost::asio::read(sock, boost::asio::buffer(msg, static_cast<size_t>(msglen)));

        std::cout << "Reply is: ";
        std::cout.write(msg, msglen) << '\n';
        std::cout << "Reply len is " << msglen;
        std::cout << "\n";
      }
    });

    send_thread.join();
    recv_thread.join();
  } catch (std::exception &e) {
    std::cerr << "Exception: " << e.what() << '\n';
  }
  return 0;
}
