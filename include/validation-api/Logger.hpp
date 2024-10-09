#pragma once

#include <spdlog/spdlog.h>

namespace validation_api {

/**
 * @brief Sets up and configures the logger using spdlog.
 * @details This function initializes the logger with a file sink and a console
 * sink. It ensures thread-safe access and supports multi-threaded logging.
 */
bool setup();

}  // namespace validation_api
