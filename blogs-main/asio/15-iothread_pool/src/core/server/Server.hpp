#ifndef SERVER_HPP
#define SERVER_HPP

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/system/detail/error_code.hpp>

#include <core/CoreExport.hpp>

#include <map>
#include <memory>
#include <string>

namespace core {

class Session;
class CORE_EXPORT Server {
public:
  Server(boost::asio::io_context& ioc, unsigned short port);

  void RemoveSession(const std::string& session_id) {
    _sessions.erase(session_id);
  }

private:
  void start_accept();
  void handle_accept(const std::shared_ptr<Session> &new_sess, const boost::system::error_code& err);

  boost::asio::io_context &_ioc;
  boost::asio::ip::tcp::acceptor _accep;

  std::map<std::string, std::shared_ptr<Session>> _sessions;
};

} // namespace core

#endif // SERVER_HPP