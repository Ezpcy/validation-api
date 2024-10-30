#include <spdlog/spdlog.h>

#include <lib/Helpers.hpp>
#include <lib/Validation.hpp>
#include <validation-api/ConfigService.hpp>

namespace validation_api {

ConfigService::ConfigService()
    : rwMutex_(),
      configs_(),
      logger_(spdlog::get("Logger") ? spdlog::get("Logger")
                                    : spdlog::default_logger()) {}
ConfigService::~ConfigService() = default;

bool ConfigService::createConfig(const std::string &name,
                                 const pugi::xml_document &doc) {
  try {
    boost::unique_lock<boost::shared_mutex> lock(rwMutex_);
    if (configs_.contains(name)) {
      logger_->error(
          "A configuration called \"{}\" already exists and it's not going to "
          "be overwritten.",
          name);
      return false;
    } else {
      auto new_doc = std::make_shared<pugi::xml_document>();
      new_doc->reset(doc);
      configs_[name] = new_doc;
      logger_->info("Configuration \"{}\" has been created.", name);
      return true;
    }
  } catch (const std::exception &e) {
    logger_->error("Error while creating configuration \"{}\": {}", name,
                   e.what());
    return false;
  }
}

void ConfigService::deleteConfig(const std::string &name) {
  try {
    boost::unique_lock<boost::shared_mutex> lock(rwMutex_);
    if (configs_.erase(name) > 0) {
      logger_->info("Configuration \"{}\" has been deleted.", name);
    } else {
      logger_->warn("Configuration \"{}\" does not exist.", name);
    }
  } catch (const std::exception &e) {
    logger_->error("Error while trying to delete configuration \"{}\": {}",
                   name, e.what());
  }
}

ConfigService::Errors ConfigService::validateConfig(
    const nlohmann::json &jsonData) {
  Errors errors;
  try {
    boost::shared_lock<boost::shared_mutex> lock(rwMutex_);
    std::string key = jsonData.begin().key();

    if (configs_.find(key) == configs_.end()) {
      errors.push_back({"Can't find configuration: ", key});
      return errors;
    }

    pugi::xml_node doc = configs_[key]->child(key.c_str());
    if (!doc) {
      errors.push_back({"Can't find child node in configuration: ", key});
      return errors;
    }

    Validation(jsonData[key], doc, errors).run();

  } catch (const std::exception &e) {
    logger_->error("Error while trying to validate request: {}", e.what());
  }

  return errors;
}

pugi::xml_document ConfigService::getConfig(const std::string &name) {
  boost::shared_lock lock(rwMutex_);

  pugi::xml_document doc;

  if (configs_.find(name) == configs_.end()) {
    logger_->warn("Configuration \"{}\" does not exist.", name);
    return doc;
  }

  doc.reset(*configs_[name]);

  return doc;
}

};  // namespace validation_api
