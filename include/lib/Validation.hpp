#pragma once

#include <nlohmann/json.hpp>
#include <pugixml.hpp>
#include <unordered_set>
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
   * @param optName optValue reqValue fieldName errors
   */
  void validate(const pugi::xml_node &node, const nlohmann::json &reqValue,
                const std::string &fieldName);

  /**
   * @brief Extract the NullOptions from a XML Configuration
   * @param doc erros nulloptions
   * @details Recursive function to find and parse all "Null" field values into
   * the NullOptions field
   */
  void extractNullOptions(const pugi::xml_node &doc);

  /*
   * @brief findJsonField
   * @param jsonObj nodeName
   * @details Recursive function to find a field in a JSON object
   */
  inline nlohmann::json findJsonField(const nlohmann::json &jsonObj,
                                      const std::string &nodeName) {
    // If the key exists at the current level, return it
    if (jsonObj.contains(nodeName)) {
      return jsonObj;
    }

    // Otherwise, look through all objects in the current level
    for (auto &[key, value] : jsonObj.items()) {
      if (value.is_object() || value.is_array()) {
        // Recursively search in nested JSON objects
        nlohmann::json result = findJsonField(value, nodeName);
        if (result != nullptr) {
          return result;
        }
      }
    }

    // Return null if the key was not found
    return nullptr;
  }

  /**
   * @brief Traverse through the json object and validate the fields.
   * @param node
   * @details Validates the json with passed xml doc object.
   * */
  void traverseAndValidate(const pugi::xml_node &node);

  /*
   * @brief Fills the `requestList_` with fields from the `request_`
   * @details Recursive function to iterate over the `request_`
   */
  inline void fillRequestList(const nlohmann::json &request) {
    for (auto &[key, value] : request.items()) {
      if (key == "Null") {
        continue;
      }

      if (value.is_object() || value.is_array()) {
        fillRequestList(value);
      } else {
        requestList_.insert(key);
      }
    }
    return;
  }

private:
  /**
   * @brief Field names from the request
   * @details Keeps track of fields and removes them when validated so we can
   * check if there any additional field which aren't part of the configuration
   */
  mutable std::unordered_set<std::string> requestList_;
  const nlohmann::json &request_;
  const pugi::xml_node &config_;
  ConfigService::Errors &errors_;
  NullOptions nullOptions_;
  std::shared_ptr<spdlog::logger> logger_;
};
} // namespace validation_api
