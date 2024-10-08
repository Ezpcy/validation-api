#pragma once

#include <deque>
#include <fstream>
#include <string>

#include "IService.hpp"

namespace validation_api {

class LoggerService {
 public:
  Logger();
  ~Logger();

  bool initialize();
  void log_info
};

}  // namespace validation_api
