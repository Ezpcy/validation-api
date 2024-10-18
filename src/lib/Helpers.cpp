#include <algorithm>
#include <boost/uuid.hpp>
#include <cctype>
#include <cstdio>
#include <exception>
#include <format>
#include <iostream>
#include <lib/Helpers.hpp>
#include <nlohmann/json.hpp>
#include <pugixml.hpp>
#include <regex>
#include <unordered_map>

#include "validation-api/ConfigService.hpp"

namespace validation_api {
using json = nlohmann::json;

// Map to decide which type needs to be validated
std::unordered_map<std::string, int> getType = {
    {"string", 1}, {"float", 2},   {"number", 3}, {"date", 4}, {"email", 5},
    {"uuid", 6},   {"boolean", 7}, {"ahv", 8},    {"iban", 9}};

// Conversion of string to lowercase
std::string toLower(std::string value) {
  std::transform(value.begin(), value.end(), value.begin(), ::tolower);
  return value;
}

// Validate a field
void validate(const std::string &optName, const std::string &optValue,
              const json &reqValue, const std::string &fieldName,
              ConfigService::Errors &errors) {
  // Validate type field
  if (optName == "type") {
    auto it = getType.find(optValue);
    if (it != getType.end()) {
      switch (it->second) {
        case 1:
          if (!reqValue.is_string()) {
            errors.push_back({std::format("Field \"{}\": ", fieldName),
                              std::format("Expected \"{}\", received {}",
                                          it->first, reqValue.type_name())});
          }
          break;
        case 2:
          if (!reqValue.is_number_float()) {
            errors.push_back({std::format("Field \"{}\": ", fieldName),
                              std::format("Expected \"{}\", received {}",
                                          it->first, reqValue.type_name())});
          }
          break;
        case 3:
          if (!reqValue.is_number()) {
            errors.push_back({std::format("Field \"{}\": ", fieldName),
                              std::format("Expected \"{}\", received {}",
                                          it->first, reqValue.type_name())});
          }
          break;
        case 4:
          const std::regex date_regex(
              R"(^\d{4}-(0[1-9]|1[0-2])-(0[1-9]|[12]\d|3[01])$)");
          std::string date = reqValue.get<std::string>();
          std::cout << date << '\n';
          if (!std::regex_match(date, date_regex)) {
            errors.push_back(
                {std::format("Field \"{}\": ", fieldName),
                 std::format(
                     "Expected \"yyyy-mm-dd\" date format, received \"{}\"",
                     date)});
          }
          break;
      }
    }
  }

  // Validate if it can be empty
}

bool isValidUuid(const std::string &uuidStr) {
  try {
    boost::uuids::string_generator gen;
    boost::uuids::uuid u = gen(uuidStr);
    return true;
  } catch (const std::exception &) {
    return false;
  }
}

void extractNullOptions(const pugi::xml_node doc, ConfigService::Errors &errors,
                        NullOptions &nullOptions) {
  // Store the name
  std::string forField = doc.name();

  for (pugi::xml_node node : doc.children()) {
    // Recursive call for child nodes
    extractNullOptions(node, errors, nullOptions);

    if (std::string(node.name()) == "Null") {
      for (pugi::xml_node nullId : node.children()) {
        // Iterate over attributes of the current <Null> child
        for (pugi::xml_attribute_iterator attr = nullId.attributes_begin();
             attr != nullId.attributes_end(); ++attr) {
          // Check if the UUID is valid
          if (isValidUuid(std::string(attr->value()))) {
            auto it = nullOptions.find(std::string(forField));

            // If the key is not found, insert it with a new vector
            if (it == nullOptions.end()) {
              std::vector<std::pair<std::string, std::string>> vec;
              vec.push_back(
                  {std::string(nullId.name()), std::string(attr->value())});
              nullOptions.insert({forField, vec});
            } else {
              // If the key exists, push the new UUID into the vector
              it->second.push_back(
                  {std::string(nullId.name()), std::string(attr->value())});
            }
          } else {
            // If the UUID is invalid, log an error
            errors.push_back(
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

// Recursive function to search for a key in the potentially nested JSON object
nlohmann::json findJsonField(const nlohmann::json &jsonObj,
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

void traverseAndValidate(const nlohmann::json jsonObj, const pugi::xml_node doc,
                         ConfigService::Errors &errors,
                         NullOptions &nullOptions) {
  for (pugi::xml_node node : doc.children()) {
    if (std::string(node.name()) == "Null") {
      continue;
    }
    // Convert to string
    std::string nodeName = node.name();
    // Check if json request has the field
    nlohmann::json jsonField = findJsonField(jsonObj, nodeName);
    if (!jsonField.is_null()) {
      // Traverse over the attributs and validate
      for (pugi::xml_attribute_iterator attr = node.attributes_begin();
           attr != node.attributes_end(); ++attr) {
        std::string optName = toLower(attr->name());
        std::string optValue = toLower(attr->value());
        std::cout << optName + " " << optValue + " " << jsonField << " "
                  << nodeName + " " << '\n';

        if (optName == "notnull") {
          auto nullOption = nullOptions.find(nodeName);
          if (nullOption != nullOptions.end()) {
            // Skip notNull option here if it is satisfied
            printf("ajie \n");
          }
        }

        validate(optName, optValue, jsonField, nodeName, errors);
      }
    } else {
      errors.push_back({std::format("Json request is missing field: "),
                        std::format("\"{}\"", nodeName)});
    }

    traverseAndValidate(jsonObj, node, errors, nullOptions);
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

void startValidation(const nlohmann::json jsonObj, const pugi::xml_node doc,
                     ConfigService::Errors &errors) {
  // Initialize nulloptions
  NullOptions nullOptions;
  // Extract nulloptions from xml
  extractNullOptions(doc, errors, nullOptions);
  for (const auto &option : nullOptions) {
    std::cout << "key: " << option.first << '\n';

    for (const auto fields : option.second) {
      std::cout << "key: " << fields.first << " value: " << fields.second
                << '\n';
    }
  }

  traverseAndValidate(jsonObj, doc, errors, nullOptions);
}
}  // namespace validation_api
