#pragma once

#include <nlohmann/json.hpp>
#include <pugixml.hpp>
#include <utility>

#include "validation-api/ConfigService.hpp"

namespace validation_api {
/**
 * @brief NullOptions
 * @details An unordered map of <string, vecto<string> to hold all uuid's with
 * their respective key value.
 */
typedef std::unordered_map<std::string,
                           std::vector<std::pair<std::string, std::string>>>
    NullOptions;

class Validation {
 public:
  Validation(const nlohmann::json &jsonObj, const pugi::xml_node &doc,
             ConfigService::Errors &errors);

  ~Validation();

  void run();

  /*
   * @brief validate a field
   * @details validates one field with the help of the xml field
   * @params optName optValue reqValue fieldName errors
   */
  void validate(const pugi::xml_node &node, const nlohmann::json &reqValue,
                const std::string &fieldName);

  /**
   * @brief Extract the NullOptions from a XML Configuration
   * @params doc erros nulloptions
   * @detail Recursive function to find and parse all "Null" field values into
   * the NullOptions field
   */
  void extractNullOptions(const pugi::xml_node &doc);

  nlohmann::json findJsonField(const nlohmann::json &jsonObj,
                               const std::string &nodeName);

  /**
   * @brief Traverse through the json object and validate the fields.
   * @params jsonObj
   * @details Validates the json with passed xml doc object.
   * */
  void traverseAndValidate(const pugi::xml_node &node);

  /**
   * @brief Start the validation process
   * @params jsonObj doc error
   * @details Start the validation process by setting up NullOptions and calling
   * the traverseAndValidate function.
   */
  void startValidation(const nlohmann::json &jsonObj, const pugi::xml_node &doc,
                       ConfigService::Errors &error);

 private:
  const nlohmann::json &request_;
  const pugi::xml_node &config_;
  ConfigService::Errors &errors_;
  NullOptions nullOptions_;
  std::shared_ptr<spdlog::logger> logger_;
};
}  // namespace validation_api
