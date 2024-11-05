#pragma once

#include <boost/asio.hpp>
#include <semaphore>

#include "validation-api/ConfigService.hpp"
#include "validation-api/IService.hpp"

namespace validation_api {
/**
 * @brief Class for exposing the application to the network
 * @detail This class uses the `boost::asio library` to create a server on a
 * specified port. This network API will be used to communicate with clients.
 */
class ValidationServer : public IService {
 public:
  ValidationServer(boost::asio::io_context& io_context, short port,
                   ConfigService& configService, short maxConnections);

  ~ValidationServer() override;

 private:
  /**
   * @brief Setup the server
   * @return true if the server is setup successfully, false
   * otherwise
   */
  bool setup() override;

  /**
   * @brief Start the server
   */
  void run() override;

  /**
   * @brief Accept a connection
   * @detail This function is called when a connection is accepted. It will try
   * to find the associated configuration, convert the request to a json object,
   * and start validating it
   */
  void accept(boost::asio::ip::tcp::socket socket);

  /*
   * @brief Write to the socket
   * @detail Write messages to the socket in a asynchronous way
   */
  void asyncWriter(const std::string& response,
                   boost::asio::ip::tcp::socket& socket);

  /**
   * @brief Stop the server
   */
  void stop() override;

  /**
   * @brief Running flag.
   * @detail Flag to indicate if the server is running. The flag is stored as an
   * atomic bool.
   */
  std::atomic<bool> running_;

  /**
   * @brief The io_context object
   * @detail The io_context object is used to perform asynchronous operations.
   */
  boost::asio::io_context& io_context_;

  /**
   * @brief The acceptor object
   * @detail The acceptor object is used to listen for incoming connection
   * requests.
   */
  boost::asio::ip::tcp::acceptor acceptor_;

  /**
   * @brief A reference to the ConfigService object
   * @detail The ConfigService object is used to retrieve the configuration
   */
  ConfigService& service_;

  /**
   * @brief A semaphore to control the number of connections
   * @detail The semaphore is used to control the number of connections that the
   * server can handle
   */
  std::counting_semaphore<> semaphore_;

  /**
   * @brief Logger object
   * @detail The logger object is used to log messages to the console and log
   * files
   */
  std::shared_ptr<spdlog::logger> logger_;

  /**
   * @brief Workguard
   * @details Keeps the server by pretending that there is work to do
   */
  std::shared_ptr<
      boost::asio::executor_work_guard<boost::asio::io_context::executor_type>>
      workGuard_;
};

}  // namespace validation_api
