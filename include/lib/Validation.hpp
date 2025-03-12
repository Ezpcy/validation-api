#pragma once

#include <boost/uuid.hpp>
#include <lib/Helpers.hpp>
#include <nlohmann/json.hpp>
#include <pugixml.hpp>
#include <unordered_set>
#include <utility>

#include "lib/ErrorBuilder.hpp"
#include "validation-api/ConfigService.hpp"

namespace validation_api {

/**
 * @brief NullOptions
 * @details An unordered map of <string, unordered_map<int, pair<string,
 * string>> to hold all uuid's with their respective key value.
 */
typedef std::unordered_map<
    std::string,
    std::unordered_map<int, std::vector<std::pair<std::string, std::string>>>>
    NullOptions;

class Validation {
public:
  Validation(const nlohmann::json &jsonObj, const pugi::xml_node &doc,
             ConfigService::Errors &errors);

  ~Validation();

  void run();

  /**
   * @brief Validates len options
  * @param fieldName
  * @param value
  * @param max
  * @param min
  * @param eq
  * @details Validates the length of the field with the max, min, and eq if they
  have a value
  */
  inline void validateConstraints(const std::string &fieldName, float value,
                                  std::optional<float> max,
                                  std::optional<float> min,
                                  std::optional<float> eq) {
    if (max && value > max.value()) {
      errorBuilder(errors_, ErrorType::MaxError, fieldName,
                   fmt::format("{}", max.value()), fmt::format("{}", value));
    }
    if (min && value < min.value()) {
      errorBuilder(errors_, ErrorType::MinError, fieldName,
                   fmt::format("{}", min.value()), fmt::format("{}", value));
    }
    if (eq && value != eq.value()) {
      errorBuilder(errors_, ErrorType::EqError, fieldName,
                   fmt::format("{}", eq.value()), fmt::format("{}", value));
    }
  }
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
  inline void extractNullOptions(const pugi::xml_node &doc) {
    // Store the name
    std::string forField = std::string(doc.name());
    int currentKey = 0;

    for (pugi::xml_node node : doc.children()) {
      // Recursive call for child nodes
      extractNullOptions(node);

      if (std::string(node.name()) == "AllowNullIf") {
        for (pugi::xml_node nullId : node.children()) {
          // Iterate over attributes of the current <Null> child
          for (pugi::xml_attribute_iterator attr = nullId.attributes_begin();
               attr != nullId.attributes_end(); ++attr) {
            // Check if the UUID is valid
            if (isValidUuid(std::string(attr->value()))) {

              auto it = nullOptions_.find(forField);

              // If the key is not found, insert it with a new vector
              if (it == nullOptions_.end()) {
                nullOptions_[forField].insert(
                    {currentKey, {{nullId.name(), attr->value()}}});

              } else {
                // If the key is found, append the new value to the vector
                nullOptions_[forField][currentKey].push_back(
                    {nullId.name(), attr->value()});
              }
            } else {
              // If the UUID is invalid, log an error
              errors_.push_back(
                  {fmt::format("XML Null option: "),
                   fmt::format("Field {} skipped because of invalid Uuid.",
                               std::string(nullId.name()))});
            }
          }
        }
        currentKey++;
      } else {
        continue;
      }
    }
  }

  /*
   * @brief findJsonField
   * @param jsonObj nodeName
   * @details Recursive function to find a field in a JSON object
   * @return std::pair<bool, nlohmann::json> We need return a pair, in case the
   * key exists but the value is a nullptr
   */
  inline std::pair<bool, nlohmann::json>
  findJsonField(const nlohmann::json &jsonObj, const std::string &nodeName) {
    // If the key exists at the current level, return it
    if (jsonObj.contains(nodeName)) {
      return std::pair(true, jsonObj[nodeName]);
    }

    // Otherwise, look through all objects in the current level
    for (auto &[key, value] : jsonObj.items()) {
      if (value.is_object() || value.is_array()) {
        // Recursively search in nested JSON objects
        nlohmann::json result = findJsonField(value, nodeName);
        if (result != std::pair(false, nullptr)) {
          return result;
        }
      }
    }

    // Return null if the key was not found
    return std::pair(false, nullptr);
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
      if (key == "AllowNullIf") {
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

  /**
   * @brief Checking Uuid values
   * @details Loops over a map of null options and checks if they exist in the
   * request
   * @param nullVec
   */
  inline bool checkNullOptionsVec(
      const std::vector<std::pair<std::string, std::string>> &nullVec) {

    for (const auto &[fieldName, fieldUuid] : nullVec) {
      // Retrieve field from JSON
      std::pair<bool, nlohmann::json> nullCheckField =
          findJsonField(request_, fieldName);

      // If the field exists and doesn't match the UUID, return false
      if (nullCheckField.second != fieldUuid) {
        return false;
      }
    }
    return true;
  }

  /**
   * @brief Checks if a json object is empty
   * @param value
   */
  inline bool isNullOrEmpty(const nlohmann::json &value) {
    return value.is_null() || value.empty() ||
           (value.is_string() && value.get<std::string>().empty());
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
