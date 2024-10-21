#include <boost/uuid.hpp>
#include <cctype>
#include <cstdio>
#include <exception>
#include <format>
#include <iostream>
#include <lib/Helpers.hpp>
#include <lib/Validation.hpp>
#include <nlohmann/json.hpp>
#include <pugixml.hpp>
#include <regex>
#include <unordered_map>

#include "validation-api/ConfigService.hpp"

namespace validation_api {
using json = nlohmann::json;

// Map to decide which type needs to be validated
std::unordered_map<std::string, int> getType = {{"date", 4}, {"email", 5},
                                                {"uuid", 6}, {"boolean", 7},
                                                {"ahv", 8},  {"iban", 9}};

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

bool Validation::isValidUuid(const std::string &uuidStr) {
  try {
    boost::uuids::string_generator gen;
    boost::uuids::uuid u = gen(uuidStr);
    return true;
  } catch (const std::exception &) {
    return false;
  }
}

// Validate a field
void Validation::validate(const pugi::xml_node &node, const json &reqValue,
                          const std::string &fieldName) {
  // Assign a bool for the "notNull" option
  pugi::xml_attribute configNullOpt = node.attribute("notNull");
  bool canBeEmpty;
  std::string notNullVal = toLower(std::string(configNullOpt.value()));

  // HERE
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
      // If the uuid is set then this field can be empty
      if (nullCheckField == value) {
        canBeEmpty = true;
      }
    }
  }

  std::string configTypeOpt = toLower(node.attribute("type").value());
  // Validate type field
  auto it = getType.find(configTypeOpt);
  if (it != getType.end()) {
    switch (it->second) {
      case 1:
        if (!reqValue.is_string()) {
          errors_.push_back({std::format("Field \"{}\": ", fieldName),
                             std::format("Expected \"{}\", received {}",
                                         it->first, reqValue.type_name())});
        } else if (reqValue.empty() || reqValue.is_null() ||
                   reqValue.get<std::string>() == "" && !canBeEmpty) {
          errors_.push_back({std::format("Field \"{}\": ", fieldName),
                             std::format("Is not allowed to be empty.")});
        }
        break;
      case 2:
        if (!reqValue.is_number_float()) {
          errors_.push_back({std::format("Field \"{}\": ", fieldName),
                             std::format("Expected \"{}\", received {}.",
                                         it->first, reqValue.type_name())});
        } else if (reqValue.empty() || reqValue.is_null() && !canBeEmpty) {
          errors_.push_back({std::format("Field \"{}\" :) ", fieldName),
                             std::format("Is not allowed to be empty.")});
        }
        break;
      case 3:
        if (!reqValue.is_number()) {
          errors_.push_back({std::format("Field \"{}\": ", fieldName),
                             std::format("Expected \"{}\", received {}.",
                                         it->first, reqValue.type_name())});
        }
        break;
      case 4:
        const std::regex date_regex(
            R"(^\d{4}-(0[1-9]|1[0-2])-(0[1-9]|[12]\d|3[01])$)");
        std::string date = reqValue.get<std::string>();
        std::cout << date << '\n';
        if (!std::regex_match(date, date_regex)) {
          errors_.push_back(
              {std::format("Field \"{}\": ", fieldName),
               std::format(
                   "Expected \"yyyy-mm-dd\" date format, received \"{}\"",
                   date)});
        }
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
  /*  for (json::const_iterator it = jsonObj.begin(); it != jsonObj.end();
   ++it) { std::string jsonKey = it.key(); json jsonValue = it.value();
   pugi::xml_node xmlNode = doc.child(jsonKey.c_str());

     if (!xmlNode) {
       errors.push_back(
           // TODO: check for <Null>
           {std::string(jsonKey), std::string(": Field can't be found.")});
       continue;
     }

     if (it->is_structured() || it->is_array() || it->is_object()) {
       traverseAndValidate(jsonObj[jsonKey], xmlNode, errors);
     } else {
       std::cout << "Key: " << jsonKey << " Value: " << jsonValue << '\n';
       for (pugi::xml_attribute_iterator attr = xmlNode.attributes_begin();
            attr != xmlNode.attributes_end(); ++attr) {
         std::string optName = toLower(attr->name());
         std::string optValue = toLower(attr->value());

         validate(optName, optValue, jsonValue, jsonKey, errors);

         std::cout << attr->name() << " " << attr->value() << " "
                   << jsonValue.type_name() << '\n';
       }

     }
   } */
}
}  // namespace validation_api