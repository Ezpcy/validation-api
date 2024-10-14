#include <spdlog/spdlog.h>

#include <boost/asio.hpp>
#include <boost/fiber/all.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <unordered_map>
#include <validation-api/ConfigService.hpp>

namespace validation_api {

ConfigService::ConfigService() = default;

// Get a configuration with a shared lock (allows concurrent reads)
nlohmann::json ConfigService::getConfig(std::string &name) {
  boost::shared_lock<boost::shared_mutex> lock(_rwMutex_);  // Shared lock
  auto it = _configs_.find(name);
  if (it != _configs_.end()) {
    nlohmann::json jsonObject;
    for (p) }
  return nlohmann::json();  // Return empty if not found
}

};  // namespace validation_api
