#include <boost/beast/core.hpp>
#include <boost/beast/core/error.hpp>
#include <boost/beast/core/ostream.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/beast/http/field.hpp>
#include <boost/beast/http/impl/read.hpp>
#include <boost/beast/http/impl/write.hpp>
#include <boost/beast/http/message_fwd.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/http/status.hpp>
#include <boost/beast/http/verb.hpp>
#include <boost/system/detail/error_code.hpp>
#include <boost/beast/http/string_body_fwd.hpp>

#include <chrono>
#include <cstddef>
#include <fstream>
#include <memory>
#include <sstream>
#include <iostream>

class HttpConnection : public std::enable_shared_from_this<HttpConnection> {
public:
  HttpConnection(boost::asio::ip::tcp::socket& sock) : _sock(std::move(sock)) { }

  void start() {
    read_request();
    check_deadline();
  }

private:
  void read_request() {
    boost::beast::http::async_read(_sock, _buffer, _request,
      [self = shared_from_this()](boost::beast::error_code errc, std::size_t) -> void {
        if (!errc) {
          self->process_request();
        }
      }
    );
  }

  void check_deadline() {
    _deadline.async_wait([self = shared_from_this()](boost::system::error_code errc) -> void {
      if (!errc) {
        if (self->_sock.is_open()) {
          self->_sock.close();
        }
      }
    });
  }

  void process_request() {
    _responce.version(_request.version());
    _responce.keep_alive(false);

    switch (_request.method()) {
      case boost::beast::http::verb::get:
        _responce.result(boost::beast::http::status::ok);
        _responce.set(boost::beast::http::field::server, "Beast demo");
        create_response();
        break;
      default:
        _responce.result(boost::beast::http::status::bad_request);
        _responce.set(boost::beast::http::field::content_type, "text/plain");
        _responce.body() = "invalid request-method";
        break;
    }

    write_response();
  }

  void create_response() {
    if (_request.target() == "/index") {
      _responce.set(boost::beast::http::field::content_type, "text/html");
      std::ifstream file("./index.html");
      if (file.is_open()) {
        std::stringstream buffer;
        buffer << file.rdbuf();
        _responce.body() = buffer.str();
        file.close();
      }
    }
  }

  void write_response() {
    _responce.prepare_payload();
    boost::beast::http::async_write(_sock, _responce,
      [self = shared_from_this()](boost::beast::error_code, std::size_t) -> void {
        self->_sock.shutdown(boost::asio::ip::tcp::socket::shutdown_send);
        self->_deadline.cancel();
      });
  }

  boost::asio::ip::tcp::socket _sock;
  boost::beast::flat_buffer _buffer{ 8192 };
  boost::beast::http::request<boost::beast::http::string_body> _request;
  boost::beast::http::response<boost::beast::http::string_body> _responce;
  boost::asio::steady_timer _deadline{_sock.get_executor(), std::chrono::seconds(10)};
};

void http_server(boost::asio::ip::tcp::acceptor& acceptor, boost::asio::ip::tcp::socket& sock) {
  acceptor.async_accept(sock,
    [&](boost::beast::error_code errc) -> void {
      if (!errc) {
        std::make_shared<HttpConnection>(sock)->start();
      }

      http_server(acceptor, sock);
    });

}

int main() {
  try {
    boost::asio::io_context io_context;
    boost::asio::ip::tcp::acceptor acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 10088));
    boost::asio::ip::tcp::socket sock(io_context);

    http_server(acceptor, sock);

    io_context.run();
  } catch (const boost::system::system_error& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }

}