#include <middleware/Logger.hpp>
#include <core/server/Server.hpp>

#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/system/detail/error_code.hpp>

int main() {
  try {
    boost::asio::io_context ioc;

    boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
    signals.async_wait([&ioc](const boost::system::error_code& err, int signal_number) -> void {
      if (!err) {
        logger.info("Received signal: {}, the server stoped.", signal_number);
        ioc.stop();
      } else {
        logger.error("Error receiving signal: {}", err.message());
      }
    });

    core::Server server{ioc, 10088};
    ioc.run();

  } catch (const boost::system::error_code& err) {
    logger.error("error code is: {}", err.value());
  }
}