#pragma once

#include <boost/uuid.hpp>
#include <cstddef>
#include <iostream>
#include <lib/Helpers.hpp>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <ostream>
#include <pugixml.hpp>
#include <unordered_set>
#include <utility>

#include "lib/ErrorBuilder.hpp"
#include "validation-api/ConfigService.hpp"

namespace validation_api {
// Map to decide which type needs to be validated
inline std::unordered_map<std::string, int> getType = {
    {"string", 1}, {"float", 2},   {"number", 3}, {"date", 4}, {"email", 5},
    {"uuid", 6},   {"boolean", 7}, {"ahv", 8},    {"iban", 9}, {"list", 10}};
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

  /*
   * @brief validation function for one
   * @details contains a switch for validation so it can be reused for recursion
   * @param
   */
  inline void
  validateOne(const int typeNumber, const nlohmann::json &reqValue,
              const std::string &fieldName, const bool &canBeEmpty,
              const auto it, const std::optional<float> &max = std::nullopt,
              const std::optional<float> &min = std::nullopt,
              const std::optional<float> &eq = std::nullopt,
              const std::optional<pugi::xml_node> &node = std::nullopt) {
    switch (typeNumber) {
    case 1:
      // Check if the value is a string
      if (!reqValue.is_string()) {
        errorBuilder(errors_, ErrorType::NotCorrectType, fieldName, it->first,
                     reqValue.type_name());
        break;
      }
      if (max.has_value() || min.has_value() || eq.has_value()) {
        // Get the string and its length
        std::string valueStr = reqValue;
        size_t length = valueStr.length();

        Validation::validateConstraints(fieldName, length, max, min, eq);
      }
      break;
    case 2:
      // Check if the value is a float
      if (!reqValue.is_number_float()) {
        std::string val;
        if (reqValue.is_string()) {
          val = reqValue.get<std::string>();
        } else if (reqValue.is_number_integer()) {
          val = std::to_string(reqValue.get<int>());
        } else {
          val = "";
        }
        errorBuilder(errors_, ErrorType::NotCorrectType, fieldName, "float",
                     val);

        break;
      }
      if (max.has_value() || min.has_value() || eq.has_value()) {
        float value = reqValue;

        Validation::validateConstraints(fieldName, value, max, min, eq);
      }
      break;
    case 3:
      // Check if the value is a number
      if (!reqValue.is_number_integer()) {
        std::string val;
        if (reqValue.is_string()) {
          val = reqValue.get<std::string>();
        } else if (reqValue.is_number_float()) {
          val = fmt::format("{}", reqValue.get<double>());
        } else {
          val = "";
        }
        errorBuilder(errors_, ErrorType::NotCorrectType, fieldName, "integer",
                     val);
        break;
      }
      if (max.has_value() || min.has_value() || eq.has_value()) {
        float value = reqValue;
        Validation::validateConstraints(fieldName, value, max, min, eq);
      }
      break;
    case 4: {
      // Check if the value is a date
      // We can skip special string types when it's empty and allowed to be
      if (canBeEmpty && reqValue.get<std::string>() == "") {
        break;
      } else {
        const std::regex date_regex(
            R"(^\d{4}-(0[1-9]|1[0-2])-(0[1-9]|[12]\d|3[01])$)");
        std::string date = reqValue.get<std::string>();
        if (!std::regex_match(date, date_regex)) {
          errorBuilder(errors_, ErrorType::NotCorrectType, fieldName, it->first,
                       date);
        }
      }
    } break;
    case 5: {
      // Check if the value is an email
      const std::regex email_regex(R"(^[a-zA-Z0-9_.+-]+@[a-zA-Z0-9-.]+$)");
      std::string email = reqValue.get<std::string>();
      if (!std::regex_match(email, email_regex)) {
        errorBuilder(errors_, ErrorType::NotCorrectType, fieldName, it->first,
                     reqValue);
      }
    } break;
    case 6: {
      // Check if the value is a uuid
      if (canBeEmpty && reqValue.get<std::string>() == "" ||
          reqValue.is_null() || reqValue.empty()) {
        break;
      }
      if (!isValidUuid(reqValue.get<std::string>())) {
        errorBuilder(errors_, ErrorType::NotCorrectType, fieldName, it->first,
                     reqValue);
      }
      break;
    case 7: {
      // Check if the value is a boolean
      if (!reqValue.is_boolean()) {
        errorBuilder(errors_, ErrorType::NotCorrectType, fieldName, it->first,
                     reqValue.type_name());
      }
    } break;
    case 8: {
      // Check if the value is a valid Ahv
      std::string val = reqValue.get<std::string>();
      if (canBeEmpty && val == "" || reqValue.is_null() || reqValue.empty()) {
        break;
      }
      if (!validateAhv(val)) {
        errorBuilder(errors_, ErrorType::NotCorrectType, fieldName, it->first,
                     reqValue);
      }
    } break;
    case 9: {
      // Check if the value is a valid Iban
      std::string val = reqValue.get<std::string>();
      if (canBeEmpty && val == "" || reqValue.is_null() || reqValue.empty()) {
        break;
      }
      if (!validateIban(val)) {
        errorBuilder(errors_, ErrorType::NotCorrectType, fieldName, it->first,
                     reqValue);
      }
    } break;
    case 10: {
      // Check if empty first
      if (canBeEmpty && (reqValue.is_null() || reqValue.empty())) {
        break;
      }
      if (!reqValue.is_array()) {
        errorBuilder(errors_, ErrorType::NotCorrectType, fieldName, it->first,
                     reqValue);
      } else {
        if (max.has_value() || min.has_value()) {
          float value = reqValue.size();
          Validation::validateConstraints(fieldName, value, max, min, eq);
        }
        std::optional<float> elementMax;
        std::optional<float> elementMin;
        std::optional<float> elementEq;
      
        std::string elementType =
            toLower(node->attribute("elementType").value());
        auto elementIt = getType.find(elementType);
        if (elementIt == getType.end()) {
          throw std::runtime_error("Unknown element type: " + elementType);
        }
        int elementNumber = elementIt->second;
        // Check for list options
        if (node->attribute("elementMax")) {
          elementMax = node->attribute("elementMax").as_float();
        }
        if (node->attribute("elementMin")) {
          elementMin = node->attribute("elementMin").as_float();
        }
        if (node->attribute("elementEq")) {
          elementEq = node->attribute("elementEq").as_float();
        }
       
        for (auto ele : reqValue.items()) {
          validateOne(elementNumber, ele.value(), fieldName,
                      canBeEmpty, elementIt, elementMax,
                      elementMin, elementEq);
        }
      }

    } break;
    default:
      break;
    }
    }
  }

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

      if (value.is_object()) {
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
