#include <spdlog/spdlog.h>

#include <boost/asio.hpp>
#include <boost/fiber/all.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <exception>
#include <nlohmann/json.hpp>
#include <unordered_map>
#include <validation-api/ConfigService.hpp>

namespace validation_api {

ConfigService::ConfigService() = default;

void ConfigService::createConfig(std::string &name, pugi::xml_document doc) {
  try {
    boost::unique_lock<boost::shared_mutex> lock(_rwMutex_);
    auto logger = spdlog::get("Logger");
    auto new_doc = std::make_shared<pugi::xml_document>();
    new_doc->reset(doc);
    _configs_[name] = new_doc;
    logger->info("Configuration named: \"{}\" has been created.", name);
  } catch (const std::exception &e) {
    spdlog::get("Logger")->error("Error while creating configuration: {}",
                                 e.what());
  }
}

void ConfigService::deleteConfig(std::string &name) {
  try {
    boost::unique_lock<boost::shared_mutex> lock(_rwMutex_);
    auto logger = spdlog::get("Logger");
    ConfigService::Configs configs;
    configs.erase(name);
  } catch (const std::exception &e) {
    spdlog::get("Logger")->error(
        "Error while trying to delete configuration: {}", e.what());
  }
}

ConfigService::Errors ConfigService::validateConfig(nlohmann::json &jsonData) {
  Errors errors;
  try {
    boost::unique_lock<boost::shared_mutex> lock(_rwMutex_);
    std::string key = jsonData.begin().key();
    std::shared_ptr<pugi::xml_document> doc = _configs_[key];

    if (!doc) {
      std::cout << "balls" << '\n';
    }

  } catch (const std::exception &e) {
    spdlog::get("Logger")->error("Error while trying to validate request: {}",
                                 e.what());
  }

  return errors;
}
};  // namespace validation_api
