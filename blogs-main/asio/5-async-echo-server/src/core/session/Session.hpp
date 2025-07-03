#ifndef SESSION_HPP
#define SESSION_HPP

#include <array>

#include <boost/system/detail/error_code.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <cstddef>
#include <core/CoreExport.hpp>
#include <global/Global.hpp>

namespace core {

class CORE_EXPORT Session {
public:
  Session(boost::asio::io_context& ioc) : _sock(ioc) {}

  void Start();

  boost::asio::ip::tcp::socket& getSocket() { return _sock; }

private:
  void handle_read(const boost::system::error_code& err, std::size_t bytes_transferred);
  void handle_write(const boost::system::error_code& err);

  boost::asio::ip::tcp::socket _sock;
  std::array<char, MAX_LENGTH> _data;
};

} // namespace core


#endif // SESSION_HPP