#include "msg.pb.h"
#include <array>
#include <boost/asio.hpp>
#include <boost/asio/detail/socket_holder.hpp>
#include <boost/asio/impl/read.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/write.hpp>
#include <boost/system/detail/error_code.hpp>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <string>
#include <winsock2.h>

#define HEAD_LENGTH 2
#define MAX_LENGTH 1024 * 2

int main() {
  try {
    boost::asio::io_context ioc;
    boost::asio::ip::tcp::endpoint enp{boost::asio::ip::make_address_v4("127.0.0.1"), 10088};
    boost::asio::ip::tcp::socket sock{ioc};
    sock.connect(enp);

    Data::MsgData msgdata;
    msgdata.set_id(1001);
    msgdata.set_data("hello, server");
    std::string raw_str;
    msgdata.SerializeToString(&raw_str);

    auto send_len = static_cast<short>(raw_str.length());
    send_len = (short)boost::asio::detail::socket_ops::host_to_network_short(static_cast<u_short>(send_len));
    std::array<char, MAX_LENGTH> send_date;
    memcpy(send_date.data(), &send_len, HEAD_LENGTH);
    memcpy(send_date.data() + HEAD_LENGTH, raw_str.c_str(), raw_str.size());
    boost::asio::write(sock, boost::asio::buffer(send_date.data(), raw_str.length() + HEAD_LENGTH));

    std::cout << "ready to receive\n";
    std::array<char, HEAD_LENGTH> head_rece;
    boost::asio::read(sock, boost::asio::buffer(head_rece.data(), HEAD_LENGTH));
    short len = 0;
    memcpy(&len, head_rece.data(), HEAD_LENGTH);
    len = (short)boost::asio::detail::socket_ops::network_to_host_short(static_cast<u_short>(len));
    std::array<char, MAX_LENGTH> recv;
    boost::asio::read(sock, boost::asio::buffer(recv.data(), static_cast<size_t>(len)));

    Data::MsgData recv_data;
    recv_data.ParseFromString(recv.data());
    std::cout << recv_data.id() << ' ' << recv_data.data() << '\n';

  } catch (const boost::system::error_code &err) {
    std::cout << "error code is: {}" << err.value() << '\n';
  }
}