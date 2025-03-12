#include "validation-api/Logger.hpp"

#include <spdlog/async.h>
#include <spdlog/logger.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <iostream>

namespace validation_api {

/**
 * @brief Sets up and configures the logger using spdlog.
 * @details This function initializes the logger with a file sink and a console
 * sink. It ensures thread-safe access and supports multi-threaded logging.
 */
bool setup_logger(std::string logpath) {
  try {
    // Initialize the logger
    spdlog::init_thread_pool(8192, 1);

    // Create console sink
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::info);
    console_sink->set_pattern("[console] [%^%l%$] %v");

    // Create error file sink
    auto error_file_sink =
        std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            logpath + "error.log", 1024 * 1024 * 5, 3);
    error_file_sink->set_level(spdlog::level::err);

    // Create info file sink
    auto info_file_sink =
        std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            logpath + "info.log", 1024 * 1024 * 5, 3);
    info_file_sink->set_level(spdlog::level::info);

    // Logger with multiple sinks
    std::vector<spdlog::sink_ptr> sinks{console_sink, error_file_sink,
                                        info_file_sink};
    auto logger = std::make_shared<spdlog::async_logger>(
        "Logger", sinks.begin(), sinks.end(), spdlog::thread_pool(),
        spdlog::async_overflow_policy::block);

    // Register the logger for global access
    spdlog::register_logger(logger);

    return true;
  } catch (const spdlog::spdlog_ex &ex) {
    std::cerr << "Log init failed: " << ex.what() << std::endl;
    return false;
  }
}
} // namespace validation_api
