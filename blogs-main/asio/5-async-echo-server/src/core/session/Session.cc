#include "Session.hpp"

#include <cstddef>
#include <boost/asio/write.hpp>

#include <middleware/Logger.hpp>

namespace core {

void Session::Start() {
  _data.fill('\0');
  _sock.async_read_some(boost::asio::buffer(_data.data(), MAX_LENGTH),
    [this](const boost::system::error_code &err, std::size_t bytes_transferred) -> void {
      handle_read(err, bytes_transferred);
    }
  );
}

void Session::handle_read(const boost::system::error_code& err, std::size_t bytes_transferred) {
  if (!err) {
    logger.info("receive data: {}\n", std::string(_data.data(), bytes_transferred));

    boost::asio::async_write(_sock, boost::asio::buffer(_data.data(), bytes_transferred),
      [this](const boost::system::error_code &write_err, size_t) -> void {
        handle_write(write_err);
      }
    );
  } else {
    logger.error("read error, err msg is: {}\n", err.message());
    delete this;
  }
}

void Session::handle_write(const boost::system::error_code& err) {
  if (!err) {
    _data.fill('\0');
    _sock.async_read_some(boost::asio::buffer(_data.data(), MAX_LENGTH),
      [this](const boost::system::error_code& read_err, size_t bytes_transferred) -> void {
        handle_read(read_err, bytes_transferred);
      }
    );
  } else {
    logger.error("write error, err msg is: {}\n", err.message());
    delete this;
  }
}

} // namespace core