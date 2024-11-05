#include <fmt/format.h>

#include <boost/asio/write.hpp>
#include <string>
#include <validation-api/ValidationServer.hpp>

// For formatting the endpoint for the logger
FMT_BEGIN_NAMESPACE

template <>
struct formatter<boost::asio::ip::tcp::endpoint> {
  constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const boost::asio::ip::tcp::endpoint& endpoint,
              FormatContext& ctx) {
    return fmt::format_to(ctx.out(), "{}:{}", endpoint.address().to_string(),
                          endpoint.port());
  }
};

FMT_END_NAMESPACE

namespace validation_api {
ValidationServer::ValidationServer(boost::asio::io_context& io_context,
                                   short port, ConfigService& configService,
                                   short maxConnections)
    : io_context_(io_context),
      acceptor_(io_context, boost::asio::ip::tcp::endpoint(
                                boost::asio::ip::tcp::v4(), port)),
      service_(configService),
      semaphore_(maxConnections),
      logger_(spdlog::get("Logger") ? spdlog::get("Logger")
                                    : spdlog::default_logger()),
      workGuard_(nullptr) {
  if (setup()) {
    run();
  } else {
    logger_->error("Server setup failed.");
  }
}

bool ValidationServer::setup() {
  try {
    // Initialize the work guard to keep the io_context_ alive
    workGuard_ = std::make_shared<boost::asio::executor_work_guard<
        boost::asio::io_context::executor_type>>(io_context_.get_executor());
    return true;
  } catch (const std::exception& e) {
    return false;
  }
}

void ValidationServer::run() {
  // Start the acceptor
  acceptor_.async_accept([this](boost::system::error_code ec,
                                boost::asio::ip::tcp::socket socket) {
    // If no error code
    if (!ec) {
      semaphore_.acquire();
      logger_->info("Client connected: {}", socket.remote_endpoint());
      accept(std::move(socket));
    }

    run();
  });
}

void ValidationServer::accept(boost::asio::ip::tcp::socket socket) {
  auto buffer = std::make_shared<boost::asio::streambuf>();

  // start an async read for the client requst
  boost::asio::async_read_until(
      socket, *buffer, "\n",
      [this, buffer, socket = std::move(socket)](boost::system::error_code ec,
                                                 std::size_t length) mutable {
        if (!ec) {
          std::istream is(buffer.get());
          std::string requestStr;
          std::getline(is, requestStr);
          try {
            nlohmann::json jsonRequest = nlohmann::json::parse(requestStr);
            ConfigService::Errors errors;

            service_.validateConfig(jsonRequest);

          } catch (nlohmann::json::parse_error& e) {
            // Extract error message from the exception
            std::string response = "Parse error: " + std::string(e.what());
            asyncWriter(response, socket);
            // Log the error
            logger_->error("{}", response);
            // Release the semaphore slot regardless of success or failure
            semaphore_.release();
          }

        } else {
          logger_->error("Failed to read request: ", ec.message());
        }
      });
}
void ValidationServer::asyncWriter(const std::string& response,
                                   boost::asio::ip::tcp::socket& socket) {
  // Asynchronously send the error response to the client
  boost::asio::async_write(
      socket, boost::asio::buffer(response),
      [this, socket = std::move(socket)](boost::system::error_code ec,
                                         std::size_t) mutable {
        if (ec) {
          // Handle any potential write error here if necessary
          logger_->error("Failed to send error response: {}", ec.message());
        }
      });
}

}  // namespace validation_api
