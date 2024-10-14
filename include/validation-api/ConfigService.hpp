#pragma once

#include <boost/asio.hpp>
#include <boost/fiber/all.hpp>
#include <boost/filesystem.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <pugixml.hpp>
#include <string>
#include <unordered_map>
#include <validation-api/IService.hpp>

namespace validation_api {

class ConfigService {
 public:
  typedef std::unordered_map<std::string, pugi::xml_document> Configs;

  ConfigService();

  nlohmann::json getConfig(std::string &name);

  void createConfig(std::string &name, pugi::xml_document doc);

  void deleteConfig(std::string &name);

  void validateConfig(nlohmann::json &jsonData);

 private:
  mutable boost::shared_mutex _rwMutex_;
  Configs _configs_;
};
}  // namespace validation_api
