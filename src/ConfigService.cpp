#include <spdlog/spdlog.h>

#include <boost/asio.hpp>
#include <boost/fiber/all.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <unordered_map>
#include <validation-api/ConfigService.hpp>

namespace validation_api {

ConfigService::ConfigService() = default;

void ConfigService::createConfig(std::string &name, pugi::xml_document doc) {
  try {
    boost::unique_lock<boost::shared_mutex> lock(_rwMutex_);
    auto logger = spdlog::get("Logger");
    _configs_[name] = std::make_shared<pugi::xml_document>(doc);
    logger->info("Configuration named: \"{}\" has been created.", name);
  } catch (const std::exception &e) {
    spdlog::get("Logger")->error("Error while creating configuration: {}",
                                 e.what());
  }
}
};  // namespace validation_api
