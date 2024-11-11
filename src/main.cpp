
#include <spdlog/spdlog.h>

#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <boost/thread.hpp>
#include <iostream>
#include <validation-api/ConfigService.hpp>
#include <validation-api/ConfigWatcher.hpp>
#include <validation-api/Logger.hpp>
#include <validation-api/ValidationServer.hpp>  // Include your server class

int main(int argc, char *argv[]) {
  std::string path = "./configs";

  // Check for command-line arguments
  if (argc > 1) {
    for (int i = 1; i < argc; i++) {
      std::string arg = argv[i];
      std::transform(arg.begin(), arg.end(), arg.begin(), ::tolower);
      if (arg == "example") {
        if (!boost::filesystem::exists(path)) {
          boost::filesystem::create_directory("./configs");
        }
        std::cout << "Example argument detected" << std::endl;
      }
    }
  }

  // Set up logger
  if (!validation_api::setup_logger()) {
    std::cerr << "Logger setup failed" << std::endl;
    return 1;
  }
  auto logger = spdlog::get("Logger");

  try {
    // Boost Asio io_context for handling async operations
    boost::asio::io_context io_context;

    // Create a work guard to keep the io_context alive
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type>
        work_guard(io_context.get_executor());

    // Initialize ConfigService
    validation_api::ConfigService service;

    // Initialize ConfigWatcher to monitor changes in the "./configs" directory
    validation_api::ConfigWatcher watcher(
        io_context, path, service,
        [&logger](const std::string &path, const std::string &action) {
          logger->info("File {} was {}", path, action);
        });

    // Initialize and run the ValidationServer on port 8080 with max connections
    short port = 8080;
    short maxConnections = 1000;
    validation_api::ValidationServer server(io_context, port, service,
                                            maxConnections);

    // Determine the number of threads
    std::size_t num_threads = std::thread::hardware_concurrency();
    // std::vector<std::thread> threads;
    boost::thread_group threads;

    // Start multiple threads, each running the io_context
    for (std::size_t i = 0; i < num_threads; ++i) {
      // threads.emplace_back([&io_context]() {
      //   io_context.run();  // Each thread handles tasks from io_context
      // });
      threads.create_thread([&io_context] { io_context.run(); });
    }

    // Output information about server and watcher
    std::cout << "Server running on port " << port
              << " and watching directory: " << path << std::endl;

    // Wait for all threads to finish
    // for (auto &thread : threads) {
    //   thread.join();
    // }

    threads.join_all();

    // Ensure a graceful shutdown
    server.stop();
    // io_context.stop();  // Stop the io_context after stopping the server
  } catch (std::exception &e) {
    std::cerr << "Exception: " << e.what() << std::endl;
  }

  return 0;
}
