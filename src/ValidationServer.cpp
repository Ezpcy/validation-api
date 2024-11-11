#include <fmt/format.h>

#include <boost/asio/steady_timer.hpp>
#include <boost/asio/write.hpp>
#include <string>
#include <validation-api/ValidationServer.hpp>

#include "lib/Helpers.hpp"

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

ValidationServer::~ValidationServer() {
  stop();  // Ensure the server is stopped and resources are released
}

bool ValidationServer::setup() {
  try {
    // Initialize the work guard to keep the io_context_ alive
    workGuard_ = std::make_shared<boost::asio::executor_work_guard<
        boost::asio::io_context::executor_type>>(io_context_.get_executor());
    return true;
  } catch (const std::exception& e) {
    logger_->error("Exception during setup: {}", e.what());
    return false;
  }
}

void ValidationServer::run() {
  if (!acceptor_.is_open()) {
    logger_->error("Acceptor is not open; cannot accept connections.");
    return;
  }

  // Start accepting connections
  acceptor_.async_accept([this](boost::system::error_code ec,
                                boost::asio::ip::tcp::socket socket) {
    if (!ec) {
      auto sharedSocket =
          std::make_shared<boost::asio::ip::tcp::socket>(std::move(socket));
      semaphore_.acquire();  // Acquire a semaphore slot for this connection
      boost::system::error_code ep_ec;
      auto endpoint = sharedSocket->remote_endpoint(ep_ec);
      if (ep_ec) {
        logger_->warn("Could not retrieve endpoint: {}", ep_ec.message());
      }
      accept(sharedSocket);  // Handle the client connection
    } else {
      logger_->error("Error during async_accept: {}", ec.message());
    }

    // Continue accepting connections
    run();
  });
}

void ValidationServer::stop() {
  // Stop accepting new connections and cancel any ongoing operations
  if (acceptor_.is_open()) {
    acceptor_.close();  // Close the acceptor to stop new connections
    logger_->info("Acceptor closed. No new connections will be accepted.");
  }

  // Release the work guard to allow io_context to finish
  if (workGuard_) {
    workGuard_.reset();
    logger_->info("Work guard released.");
  }

  // Ensure all pending connections are completed
  io_context_.stop();
  logger_->info("Server stopped.");
}

void ValidationServer::accept(
    std::shared_ptr<boost::asio::ip::tcp::socket> socket) {
  auto buffer = std::make_shared<boost::asio::streambuf>();
  auto timer =
      std::make_shared<boost::asio::steady_timer>(socket->get_executor());

  // Setting the timeout timer
  timer->expires_after(std::chrono::seconds(60));

  // Start the timeout timer
  timer->async_wait([this, socket, timer](const boost::system::error_code& ec) {
    if (!ec) {
      // timer expired
      logger_->warn("{}: Connection timed out due to inactivity.",
                    socket->remote_endpoint());
      socket->close();
      semaphore_.release();
    }
  });

  // Start an async read operation for the client request
  boost::asio::async_read_until(
      *socket, *buffer, "\n",
      [this, buffer, socket, timer](boost::system::error_code ec,
                                    std::size_t length) mutable {
        if (!ec) {
          // Reset timer
          timer->cancel();

          std::istream is(buffer.get());
          std::string requestStr;
          std::getline(is, requestStr);

          if (requestStr.empty()) {
            // Handle empty request case
            std::string response = "Error: Received empty request.\n";
            asyncWriter(response, socket);
            logger_->error("Received empty request.");
            semaphore_.release();
            return;
          }

          ConfigService::Errors errors;
          std::string keyName;

          try {
            // Parse the JSON request
            nlohmann::json jsonRequest = nlohmann::json::parse(requestStr);
            keyName = jsonRequest.begin().key();  // Extract the first key

            // Validate the JSON using ConfigService
            errors = service_.validateConfig(jsonRequest);
          } catch (std::exception& e) {
            // Handle JSON parsing error
            std::string response =
                "Parse error: " + std::string(e.what()) + "\n";
            asyncWriter(response, socket);
            logger_->error("Parse error: {}", e.what());
            semaphore_.release();
            return;
          }

          // Process validation results
          if (errors.empty()) {
            nlohmann::json response;
            response[keyName] = "Ok";
            asyncWriter(response.dump() + "\n", socket);
          } else {
            // Convert errors to JSON and send them to the client
            nlohmann::json errorResponse = errorsToJson(errors);
            nlohmann::json errorWrap;
            errorWrap["Error"] = errorResponse;

            logger_->warn(
                "Validation request from {} with configuration {} failed",
                socket->remote_endpoint(), keyName);
            asyncWriter(errorWrap.dump() + "\n", socket);
          }

          // Release semaphore after processing request
          semaphore_.release();
        } else {
          // Log and handle read error
          logger_->error("Failed to read request: {}", ec.message());
          semaphore_.release();
        }
      });
}

void ValidationServer::asyncWriter(
    const std::string& response,
    std::shared_ptr<boost::asio::ip::tcp::socket> socket) {
  // Asynchronously send the response to the client
  boost::asio::async_write(
      *socket, boost::asio::buffer(response),
      [this, socket](boost::system::error_code ec, std::size_t) mutable {
        if (ec) {
          logger_->error("Failed to send response: {}", ec.message());
        }
        // The connection will be closed automatically when socket goes out of
        // scope
      });
}

}  // namespace validation_api
