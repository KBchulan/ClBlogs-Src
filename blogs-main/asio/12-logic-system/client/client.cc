#include <boost/asio.hpp>
#include <boost/asio/ip/address.hpp>
#include <iostream>
#include <json/json.h>
#include <json/reader.h>
#include <json/value.h>
#include <json/writer.h>
#include <sstream>
#include <string>

#define HEAD_LENGTH 2
#define MAX_LENGTH 1024 * 2

int main() {
  try {
    boost::asio::io_context ioc;
    boost::asio::ip::tcp::endpoint enp{boost::asio::ip::make_address_v4("127.0.0.1"), 10088};
    boost::asio::ip::tcp::socket sock{ioc};
    sock.connect(enp);

    Json::Value root;
    root["id"] = 1001;
    root["data"] = "hello, server";
    Json::StreamWriterBuilder builder;
    std::string raw_str = Json::writeString(builder, root);

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

    Json::CharReaderBuilder read_builder;
    std::stringstream sss{recv.data()};
    Json::Value read;
    std::string errors;
    Json::parseFromStream(read_builder, sss, &read, &errors);
    std::cout << "id is: " << read["id"].asString() << " data is: " << read["data"].asString() << '\n';

  } catch (const boost::system::error_code &err) {
    std::cout << "error code is: {}" << err.value() << '\n';
  }
}