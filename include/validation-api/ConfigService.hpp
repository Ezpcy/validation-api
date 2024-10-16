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
#include <utility>
#include <validation-api/IService.hpp>

namespace validation_api {

/**
 * @brief ConfigService class
 *
 * @details This class is responsible for managing the configuration files.
 * It provides methods to create, delete, get and validate configuration files.
 */
class ConfigService {
 public:
  /**
   * @brief Configs typedef
   *
   * @details This typedef is used to store the configuration files in a map.
   */
  typedef std::unordered_map<std::string, std::shared_ptr<pugi::xml_document>>
      Configs;

  /**
   * @brief Errors typedef
   *
   * @details This typedef is used to store the errors found in the
   * configuration files.
   */
  typedef std::vector<std::pair<std::string, std::string>> Errors;

  /**
   * @brief ConfigService constructor
   */
  ConfigService();

  /**
   * @brief ConfigService destructor
   */
  ~ConfigService();

  /**
   * @brief getConfig method
   * @params name  The name of the configuration file.
   * @return nlohmann::json object
   *
   * @details This method returns the configuration files.
   *
   * @return Configs
   */
  nlohmann::json getConfig(const std::string &name);

  /**
   * @brief getConfigs method
   * @details Insert the configuration files into the map.
   * @params name  The name of the configuration file.
   * @params doc  The configuration file.
   *
   */
  void createConfig(const std::string &name, const pugi::xml_document doc);

  /**
   * @brief getConfigs method
   *
   * @details This method delete a config from the map if it exists.
   *
   */
  void deleteConfig(const std::string &name);

  /**
   * @brief getConfigs method
   * @details Validates a json object through a configuration if it exists.
   * @see traverseAndValidate
   * @return Errors
   */
  Errors validateConfig(const nlohmann::json &jsonData);

 private:
  /**
   * @brief _rwMutex_ member
   *
   * @details This member is used to lock the map of configuration files.
   */
  mutable boost::shared_mutex _rwMutex_;

  /**
   * @brief _configs_ member
   *
   * @details This member is used to store the configuration files.
   */
  Configs _configs_;
};
}  // namespace validation_api
