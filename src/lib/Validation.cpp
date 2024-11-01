#include <boost/uuid.hpp>
#include <cctype>
#include <cstdio>
#include <iostream>
#include <lib/Helpers.hpp>
#include <lib/Validation.hpp>
#include <nlohmann/json.hpp>
#include <pugixml.hpp>
#include <regex>
#include <unordered_map>

#include "lib/ErrorBuilder.hpp"
#include "validation-api/ConfigService.hpp"

namespace validation_api {
using json = nlohmann::json;

// Map to decide which type needs to be validated
std::unordered_map<std::string, int> getType = {
    {"string", 1}, {"float", 2},   {"number", 3}, {"date", 4}, {"email", 5},
    {"uuid", 6},   {"boolean", 7}, {"ahv", 8},    {"iban", 9}};

Validation::Validation(const json &jsonObj, const pugi::xml_node &doc,
                       ConfigService::Errors &errors)
    : request_(jsonObj),
      config_(doc),
      errors_(errors),
      nullOptions_(),
      logger_(spdlog::get("Logger") ? spdlog::get("Logger")
                                    : spdlog::default_logger()) {
  // Extract nulloptions from xml
  extractNullOptions(config_);
  for (const auto &option : nullOptions_) {
    std::cout << "key: " << option.first << '\n';

    for (const auto fields : option.second) {
      std::cout << "key: " << fields.first << " value: " << fields.second
                << '\n';
    }
  }
};

Validation::~Validation() = default;

void Validation::run() { traverseAndValidate(config_); }

// Validate a field
void Validation::validate(const pugi::xml_node &node, const json &reqValue,
                          const std::string &fieldName) {
  // Assign a bool for the "notNull" option
  pugi::xml_attribute configNullOpt = node.attribute("notNull");
  // Optional floats for max, min, and eq
  std::optional<float> max;
  std::optional<float> min;
  std::optional<float> eq;

  // Check if each attribute exists before assigning
  if (node.attribute("max")) {
    max = node.attribute("max").as_float();
  }
  if (node.attribute("min")) {
    min = node.attribute("min").as_float();
  }
  if (node.attribute("eq")) {
    eq = node.attribute("eq").as_float();
  }
  bool canBeEmpty;
  std::string notNullVal = toLower(std::string(configNullOpt.value()));

  // Set the bool accordingly to the attribute
  if (notNullVal == "true") {
    canBeEmpty = false;
  } else if (notNullVal == "false") {
    canBeEmpty = true;
    // Default
  } else {
    canBeEmpty = false;
  }

  // Check for nullOptions
  if (nullOptions_.find(fieldName) != nullOptions_.end()) {
    auto it = nullOptions_.find(fieldName)->second;
    // Iterate over the null options
    for (const auto &[key, value] : it) {
      // Look up the values and compare them
      json nullCheckField = findJsonField(request_, key);
      std::cout << nullCheckField << " " << value << '\n';
      // If the uuid is set then this field can be empty
      if (nullCheckField == value) {
        canBeEmpty = true;
      }
    }
  }

  // Check if field is emtpy
  if (reqValue.empty() || reqValue.is_null() || reqValue == "" && !canBeEmpty) {
    errors_.push_back(
        {ErrorBuilder(ErrorType::ValidaionEmptyError, fieldName).build()});
    return;
  }

  std::string configTypeOpt = toLower(node.attribute("type").value());
  // Validate type field
  auto it = getType.find(configTypeOpt);
  if (it != getType.end()) {
    switch (it->second) {
      case 1:
        // Chedk if the value is a string
        if (!reqValue.is_string()) {
          errors_.push_back({ErrorBuilder(ErrorType::ValidationError, fieldName)
                                 .setSecondMsg(it->first, reqValue.type_name())
                                 .build()});
        } else if (max.has_value()) {
          // Get the string and its length
          std::string valueStr = reqValue;
          size_t length = valueStr.length();

          // Check against 'max' constraint
          if (max.has_value() && length > max.value()) {
            errors_.push_back(
                {ErrorBuilder(ErrorType::ValidationError, fieldName)
                     .setSecondMsg(
                         std::format("max length of {},", max.value()),
                         valueStr)
                     .build()});
          }

          // Check against 'min' constraint
          if (min.has_value() && length < min.value()) {
            errors_.push_back(
                {ErrorBuilder(ErrorType::ValidationError, fieldName)
                     .setSecondMsg(
                         std::format("min length of {}.", min.value()),
                         valueStr)
                     .build()});
          }

          // Check against 'eq' constraint (exact length)
          if (eq.has_value() && length != eq.value()) {
            errors_.push_back(
                {ErrorBuilder(ErrorType::ValidationError, fieldName)
                     .setSecondMsg(
                         std::format("exact length of {}.", eq.value()),
                         valueStr)
                     .build()});
          }
        }
        break;
      case 2:
        // Check if the value is a float
        if (!reqValue.is_number_float()) {
          errors_.push_back({ErrorBuilder(ErrorType::ValidationError, fieldName)
                                 .setSecondMsg(it->first, reqValue.type_name())
                                 .build()});
          float value = reqValue;

          // Check against 'max' constraint
          if (max.has_value() && value > max.value()) {
            errors_.push_back(
                {ErrorBuilder(ErrorType::ValidationError, fieldName)
                     .setSecondMsg(
                         std::format("value lower than {}", max.value()),
                         std::format("{:.2f}", value))
                     .build()});
          }

          // Check against 'min' constraint
          if (min.has_value() && value < min.value()) {
            errors_.push_back(
                {ErrorBuilder(ErrorType::ValidationError, fieldName)
                     .setSecondMsg(
                         std::format("value higher than {}.", min.value()))
                     .build()});
          }

          // Check against 'eq' constraint (exact value)
          if (eq.has_value() && value != eq.value()) {
            errors_.push_back(
                {ErrorBuilder(ErrorType::ValidationError, fieldName)
                     .setSecondMsg(
                         std::format("exact value of {}.", eq.value()))
                     .build()});
          }
        }
        break;
      case 3:
        // Check if the value is a number
        if (!reqValue.is_number_integer()) {
          errors_.push_back({ErrorBuilder(ErrorType::ValidationError, fieldName)
                                 .setSecondMsg(it->first, reqValue.type_name())
                                 .build()});
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
            errors_.push_back(
                {ErrorBuilder(ErrorType::ValidationError, fieldName)
                     .setSecondMsg(std::string("\"yyyy-mm-dd\" date format"),
                                   date)
                     .build()});
          };
        }
        break;
      }
      case 5: {
        // Check if the value is an email
        const std::regex email_regex(R"(^[a-zA-Z0-9_.+-]+@[a-zA-Z0-9-.]+$)");
        std::string email = reqValue.get<std::string>();
        if (!std::regex_match(email, email_regex)) {
          errors_.push_back({ErrorBuilder(ErrorType::ValidationError, fieldName)
                                 .setSecondMsg(it->first, reqValue)
                                 .build()});
        }
        break;
      }
      case 6: {
        // Check if the value is a uuid
        if (canBeEmpty && reqValue.get<std::string>() == "" ||
            reqValue.is_null() || reqValue.empty()) {
          break;
        }
        if (!isValidUuid(reqValue.get<std::string>())) {
          errors_.push_back({ErrorBuilder(ErrorType::ValidationError, fieldName)
                                 .setSecondMsg(it->first, reqValue)
                                 .build()});
        }
        break;
      }
      case 7: {
        // Check if the value is a boolean
        if (!reqValue.is_boolean()) {
          errors_.push_back({ErrorBuilder(ErrorType::ValidationError, fieldName)
                                 .setSecondMsg(it->first, reqValue.type_name())
                                 .build()});
        }
        break;
      }
      case 8: {
        // Check if the value is a valid Ahv
        std::string val = reqValue.get<std::string>();
        if (canBeEmpty && val == "" || reqValue.is_null() || reqValue.empty()) {
          break;
        }
        if (!validateAhv(val)) {
          errors_.push_back({ErrorBuilder(ErrorType::ValidationError, fieldName)
                                 .setSecondMsg(it->first, reqValue)
                                 .build()});
        }
        break;
      }
      case 9: {
        // Check if the value is a valid Iban
        std::string val = reqValue.get<std::string>();
        if (canBeEmpty && val == "" || reqValue.is_null() || reqValue.empty()) {
          break;
        }
        if (!validateIban(val)) {
          errors_.push_back({ErrorBuilder(ErrorType::ValidationError, fieldName)
                                 .setSecondMsg(it->first, reqValue)
                                 .build()});
        }
        break;
      }
      default:
        break;
    }
  }

  // Validate if it can be empty
}

void Validation::extractNullOptions(const pugi::xml_node &doc) {
  // Store the name
  std::string forField = std::string(doc.name());

  for (pugi::xml_node node : doc.children()) {
    // Recursive call for child nodes
    extractNullOptions(node);

    if (std::string(node.name()) == "Null") {
      for (pugi::xml_node nullId : node.children()) {
        // Iterate over attributes of the current <Null> child
        for (pugi::xml_attribute_iterator attr = nullId.attributes_begin();
             attr != nullId.attributes_end(); ++attr) {
          // Check if the UUID is valid
          if (isValidUuid(std::string(attr->value()))) {
            auto it = nullOptions_.find(std::string(forField));

            // If the key is not found, insert it with a new vector
            if (it == nullOptions_.end()) {
              std::vector<std::pair<std::string, std::string>> vec;
              vec.push_back(
                  {std::string(nullId.name()), std::string(attr->value())});
              nullOptions_.insert({forField, vec});
            } else {
              // If the key exists, push the new UUID into the vector
              it->second.push_back(
                  {std::string(nullId.name()), std::string(attr->value())});
            }
          } else {
            // If the UUID is invalid, log an error
            errors_.push_back(
                {std::format("XML \"Null\" option: "),
                 std::format("Field \"{}\" skipped because of invalid Uuid.",
                             std::string(nullId.name()))});
          }
        }
      }
    } else {
      continue;
    }
  }
}

// Recursive function to search for a key in the potentially nested JSON
// object
json Validation::findJsonField(const json &jsonObj,
                               const std::string &nodeName) {
  // If the key exists at the current level, return it
  if (jsonObj.contains(nodeName)) {
    return jsonObj[nodeName];
  }

  // Otherwise, look through all objects in the current level
  for (auto &[key, value] : jsonObj.items()) {
    if (value.is_object()) {
      // Recursively search in nested JSON objects
      nlohmann::json result = findJsonField(value, nodeName);
      if (!result.is_null()) {
        return result;
      }
    }
  }

  // Return null if the key was not found
  return nullptr;
}

// Recursive function to traverse and validate the request
void Validation::traverseAndValidate(const pugi::xml_node &node) {
  for (pugi::xml_node field : node.children()) {
    if (std::string(field.name()) == "Null") {
      continue;
    }
    // Convert to string
    std::string nodeName = field.name();
    // Check if json request has the field
    nlohmann::json jsonField = findJsonField(request_, nodeName);
    if (!jsonField.is_null()) {
      validate(field, jsonField, nodeName);
    } else {
      errors_.push_back({std::format("Json request is missing field: "),
                         std::format("\"{}\"", nodeName)});
    }

    traverseAndValidate(field);
  }
}
}  // namespace validation_api
