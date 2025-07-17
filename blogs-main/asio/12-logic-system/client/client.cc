#include <boost/asio.hpp>
#include <boost/asio/ip/address.hpp>
#include <cstring>
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
    root["test"] = "test str";
    root["data"] = "hello, server";
    Json::StreamWriterBuilder builder;
    std::string raw_str = Json::writeString(builder, root);

    short data_id = 1001;
    data_id = (short)boost::asio::detail::socket_ops::host_to_network_short(static_cast<u_short>(data_id));
    auto send_len = static_cast<short>(raw_str.length());
    send_len = (short)boost::asio::detail::socket_ops::host_to_network_short(static_cast<u_short>(send_len));
    std::array<char, MAX_LENGTH> send_date;
    memcpy(send_date.data(), &data_id, HEAD_LENGTH);
    memcpy(send_date.data() + HEAD_LENGTH, &send_len, HEAD_LENGTH);
    memcpy(send_date.data() + HEAD_LENGTH + HEAD_LENGTH, raw_str.c_str(), raw_str.size());
    boost::asio::write(sock, boost::asio::buffer(send_date.data(), raw_str.length() + HEAD_LENGTH + HEAD_LENGTH));

    std::cout << "ready to receive\n";
    std::array<char, HEAD_LENGTH> head_id;
    boost::asio::read(sock, boost::asio::buffer(head_id.data(), HEAD_LENGTH));
    short head_id_local = 0;
    memcpy(&head_id_local, head_id.data(), HEAD_LENGTH);
    head_id_local = (short)boost::asio::detail::socket_ops::network_to_host_short(static_cast<u_short>(head_id_local));
    std::cout << "head id is: " << head_id.data() << '\n';

    // 读取头部长度
    std::array<char, HEAD_LENGTH> head_len;
    boost::asio::read(sock, boost::asio::buffer(head_len.data(), HEAD_LENGTH));
    short len = 0;
    memcpy(&len, head_len.data(), HEAD_LENGTH);
    len = (short)boost::asio::detail::socket_ops::network_to_host_short(static_cast<u_short>(len));
    std::cout << "head len is: " << len << '\n';
    std::array<char, MAX_LENGTH> recv;
    boost::asio::read(sock, boost::asio::buffer(recv.data(), static_cast<size_t>(len)));

    Json::CharReaderBuilder read_builder;
    std::stringstream sss{recv.data()};
    Json::Value read;
    std::string errors;
    Json::parseFromStream(read_builder, sss, &read, &errors);
    std::cout << "test is: " << read["test"].asString() << " data is: " << read["data"].asString() << '\n';

  } catch (const boost::system::error_code &err) {
    std::cout << "error code is: {}" << err.value() << '\n';
  }
}