#include <lib/Helpers.hpp>
#include <lib/Validation.hpp>
#include <string>
#include <unordered_map>

#include "lib/ErrorBuilder.hpp"

namespace validation_api {
using json = nlohmann::json;
Validation::Validation(const json &jsonObj, const pugi::xml_node &doc,
                       ConfigService::Errors &errors)
    : request_(jsonObj), config_(doc), errors_(errors), nullOptions_(),
      logger_(spdlog::get("Logger") ? spdlog::get("Logger")
                                    : spdlog::default_logger()) {
  // Extract nulloptions from xml
  extractNullOptions(config_);
};

Validation::~Validation() = default;

void Validation::run() {
  fillRequestList(request_);

  traverseAndValidate(config_);

  if (!requestList_.empty()) {
    for (const auto &field : requestList_) {
      errorBuilder(errors_, ErrorType::RequestFieldNotFound, field);
    }
  }
}

// Recursive function to traverse and validate the request
void Validation::traverseAndValidate(const pugi::xml_node &node) {
  for (pugi::xml_node field : node.children()) {
    // Convert to string
    std::string nodeName = field.name();
    // Check if it is a Null field
    if (nodeName == "AllowNullIf") {
      continue;
    }

    // Check if json request has the field
    std::pair<bool, json> jsonField = findJsonField(request_, nodeName);

    if (jsonField.first) {
      validate(field, jsonField.second, nodeName);
      requestList_.erase(nodeName);
    } else {
      errorBuilder(errors_, ErrorType::MissingField, nodeName);
      continue;
    }

    traverseAndValidate(field);
  }
}

// Validate a field
void Validation::validate(const pugi::xml_node &node, const json &reqValue,
                          const std::string &fieldName) {
  try {
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

        // Check all fields
        if (checkNullOptionsVec(value)) {
          canBeEmpty = true;
          break;
        }
      }
    }

    // Check if field is empty
    if (isNullOrEmpty(reqValue)) {
      if (!canBeEmpty) {
        errorBuilder(errors_, ErrorType::CannotBeEmpty, fieldName);
      }
      return;
    }

    std::string configTypeOpt = toLower(node.attribute("type").value());
    // Validate type field
    auto it = getType.find(configTypeOpt);
    if (it != getType.end()) {
      int typeNumber = it->second;
      std::optional<int> elementNumber;
      Validation::validateOne(typeNumber, reqValue, fieldName, canBeEmpty, it,
                              max, min, eq, false, node);
    }
  } catch (const std::exception &e) {
    errors_[fieldName] += std::string(e.what());
  }
}

} // namespace validation_api
