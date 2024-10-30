#pragma once

#include <spdlog/spdlog.h>

#include <boost/thread.hpp>
#include <nlohmann/json.hpp>
#include <pugixml.hpp>

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
   * @return pugi::xml_document
   *
   * @details This method returns the configuration
   *
   * @return Configs
   */
  pugi::xml_document getConfig(const std::string &name);

  /**
   * @brief getConfigs method
   * @details Insert the configuration files into the map.
   * @params name  The name of the configuration file.
   * @params doc  The configuration file.
   *
   */
  bool createConfig(const std::string &name, const pugi::xml_document &doc);

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
   * @brief rwMutex_ member
   *
   * @details This member is used to lock the map of configuration files.
   */
  mutable boost::shared_mutex rwMutex_;

  /**
   * @brief configs_ member
   *
   * @details This member is used to store the configuration files.
   */
  Configs configs_;

  /**
   * @brief logger_ member
   *
   * @details This member is used to log messages.
   */
  std::shared_ptr<spdlog::logger> logger_;
};
}  // namespace validation_api
