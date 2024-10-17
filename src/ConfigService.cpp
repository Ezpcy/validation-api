#include <spdlog/spdlog.h>

#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/thread.hpp>
#include <exception>
#include <lib/Helpers.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>
#include <validation-api/ConfigService.hpp>

namespace validation_api {

ConfigService::ConfigService() = default;
ConfigService::~ConfigService() = default;

void ConfigService::createConfig(const std::string &name,
                                 const pugi::xml_document &doc) {
  try {
    boost::unique_lock<boost::shared_mutex> lock(_rwMutex_);
    std::cout << "here" << '\n';
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

void ConfigService::deleteConfig(const std::string &name) {
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

ConfigService::Errors ConfigService::validateConfig(
    const nlohmann::json &jsonData) {
  Errors errors;
  try {
    boost::unique_lock<boost::shared_mutex> lock(_rwMutex_);
    std::string key = jsonData.begin().key();
    pugi::xml_node doc = _configs_[key]->child(key.c_str());

    if (!doc) {
      errors.push_back(
          {std::string("Can't find configuration: "), std::string(key)});
      return errors;
    }
    boost::thread validationThread([this, &jsonData, &doc, &errors, &key]() {
      validation_api::traverseAndValidate(jsonData[key], doc, errors);
    });

    validationThread.join();

  } catch (const std::exception &e) {
    spdlog::get("Logger")->error("Error while trying to validate request: {}",
                                 e.what());
  }

  return errors;
}
};  // namespace validation_api
