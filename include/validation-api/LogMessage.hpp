#pragma once

#include <string>

namespace validation_api {
/**
 * @brief Enum class to define the log levels.
 * @details The log levels are used to categorize log messages. The levels
 * are used to determine the severity of the message.
 *
 * `INFO` is used for general information messages.
 * `WARNING` is used for messages that indicate a potential problem.
 * `ERROR` is used for messages that indicate a problem that needs to be
 *
 * @see LoggerService
 */
enum class LogLevel { INFO, WARNING, ERROR };

struct LogMessage {
  LogLevel level;
  std::string message;
  bool display_to_console;

  LogMessage(LogLevel level, std::string message, bool display_to_console)
      : level(level),
        message(message),
        display_to_console(display_to_console) {}
};
}  // namespace validation_api
