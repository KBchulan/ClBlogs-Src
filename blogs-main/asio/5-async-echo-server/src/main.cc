#include "middleware/Logger.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/system/detail/error_code.hpp>
#include <core/server/Server.hpp>

int main() {
  try {
    boost::asio::io_context ioc;
    core::Server server{ioc, 10088};
    ioc.run();
  } catch (const boost::system::error_code& err) {
    logger.error("error code is: {}\n", err.value());
  }
}