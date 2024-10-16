#include <algorithm>
#include <cctype>
#include <format>
#include <lib/Helpers.hpp>
#include <nlohmann/json.hpp>
#include <pugixml.hpp>

#include "validation-api/ConfigService.hpp"

namespace validation_api {
using json = nlohmann::json;

std::unordered_map<std::string, int> getType = {
    {"string", 1}, {"float", 2},   {"number", 3}, {"date", 4}, {"email", 5},
    {"uuid", 6},   {"boolean", 7}, {"ahv", 8},    {"iban", 9}};

std::string toLower(std::string value) {
  std::transform(value.begin(), value.end(), value.begin(), ::tolower);
  return value;
}

void validate(const std::string &optName, const std::string &optValue,
              const json &reqValue, const std::string &fieldName,
              ConfigService::Errors &errors) {
  if (optName == "type") {
    auto it = getType.find(optValue);
    if (it != getType.end()) {
      switch (it->second) {
        case 1:
          if (!reqValue.is_string()) {
            errors.push_back({std::format("{}: ", fieldName),
                              std::format("Expected String, received {}",
                                          reqValue.type_name())});
          }
      }
    }
  }
}

void traverseAndValidate(const nlohmann::json jsonObj, const pugi::xml_node doc,
                         ConfigService::Errors &errors) {
  for (json::const_iterator it = jsonObj.begin(); it != jsonObj.end(); ++it) {
    std::string jsonKey = it.key();
    json jsonValue = it.value();
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

        std::cout << attr->name() << " " << attr->value()
                  << jsonValue.type_name() << '\n';
      }
    }
  }
  if (!errors.empty()) {
    for (const auto &error : errors) {
      std::cout << error.first << error.second << '\n';
    }
  }
}
}  // namespace validation_api
