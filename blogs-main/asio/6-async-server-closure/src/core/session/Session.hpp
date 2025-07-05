#ifndef SESSION_HPP
#define SESSION_HPP

#include <array>
#include <memory>
#include <cstddef>

#include <boost/system/detail/error_code.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <core/CoreExport.hpp>
#include <global/Global.hpp>

namespace core {

class Server;
class CORE_EXPORT Session : public std::enable_shared_from_this<Session> {
public:
  Session(boost::asio::io_context& ioc, Server* server) : _sock(ioc), _server(server) {
    boost::uuids::uuid uuid = boost::uuids::random_generator_mt19937()();
    _uuid = boost::uuids::to_string(uuid);
  }

  void Start();

  boost::asio::ip::tcp::socket& getSocket() { return _sock; }
  std::string getUUid() { return _uuid; }

private:
  void handle_read(const boost::system::error_code& err, std::size_t bytes_transferred);
  void handle_write(const boost::system::error_code& err);

  boost::asio::ip::tcp::socket _sock;
  std::array<char, MAX_LENGTH> _data;

  Server *_server;
  std::string _uuid;
};

} // namespace core


#endif // SESSION_HPP