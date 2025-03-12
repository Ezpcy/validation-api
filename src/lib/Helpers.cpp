#include <fmt/core.h>
#include <lib/Helpers.hpp>
#include <string>
#include <unordered_set>

#include "validation-api/ConfigService.hpp"

namespace validation_api {
void validateXmlConfig(const pugi::xml_node &node,
                       validation_api::ConfigService::Errors &errors) {
  std::unordered_set<std::string> validTypes = {
      "string", "float",   "number", "date", "email",
      "uuid",   "boolean", "ahv",    "iban", "list"};

  const std::unordered_set<std::string> lenCheckType = {"string", "number",
                                                        "list", "float"};

  // Recursively traverse child nodes
  for (pugi::xml_node child = node.first_child(); child;
       child = child.next_sibling()) {
    validateXmlConfig(child, errors);
  }

  std::string nodeName = node.name();

  // Validate type attribute
  pugi::xml_attribute attrType = node.attribute("type");
  if (attrType) {
    std::string typeName = toLower(attrType.value());
    if (validTypes.find(typeName) == validTypes.end()) {
      insertToJson(errors, nodeName,
                   fmt::format("Type '{}' is not valid", typeName));
    }
  }

  // Validate len options (max, min, eq)
  std::optional<float> max, min, eq;

  auto validateLenOption = [&](const pugi::xml_attribute &attr,
                               const char *optionName,
                               std::optional<float> &value) {
    if (attr) {
      std::string val = attr.value();
      if (val.empty()) {
        insertToJson(errors, nodeName,
                     fmt::format("{} option cannot be empty", optionName));
        return;
      }
      float parsedValue;
      if (!isFloat(val, parsedValue)) {
        insertToJson(
            errors, nodeName,
            fmt::format("Expected a number for {} option, received '{}'",
                        optionName, val));
        return;
      }
      value = parsedValue;
    }
  };

  validateLenOption(node.attribute("max"), "max", max);
  validateLenOption(node.attribute("min"), "min", min);
  validateLenOption(node.attribute("eq"), "eq", eq);

  if (toLower(attrType.value()) == "list") {

    // check list option: elementType, elementMax, elementMin
    pugi::xml_attribute attrElementType = node.attribute("elementType");
    if (!attrElementType) {
      insertToJson(errors, nodeName,
                   "List type must have elementType attribute");
    } else {
      std::string elementType = toLower(attrElementType.value());
      if (validTypes.find(elementType) == validTypes.end()) {
        insertToJson(errors, nodeName,
                     fmt::format("Type '{}' is not valid", elementType));
      }
    }

    std::optional<float> elementMax, elementMin;

    auto validateElementLenOption = [&](const pugi::xml_attribute &attr,
                                        const char *optionName,
                                        std::optional<int> &value) {
      validateLenOption(node.attribute("elementMax"), "elementMax", elementMax);
      validateLenOption(node.attribute("elementMin"), "elementMin", elementMin);
      if (node.attribute("eq")) {
        insertToJson(errors, nodeName,
                     fmt::format("List type doesn't support eq option"));
      }
    };
  }

  if ((max || min || eq) &&
      (attrType &&
       lenCheckType.find(toLower(attrType.value())) == lenCheckType.end())) {
    insertToJson(
        errors, nodeName,
        fmt::format("Type '{}' cannot have length options (max, min, eq)",
                    attrType.value()));
  }

  if (max && min && *max < *min) {
    insertToJson(
        errors, nodeName,
        fmt::format("Min value ({}) cannot be greater than max value ({})",
                    *min, *max));
  }

  if ((max || min) && eq) {
    insertToJson(errors, nodeName,
                 "Eq option cannot be set if max or min are specified");
  }

  // Validate notNull option
  pugi::xml_attribute attrNotNull = node.attribute("notNull");
  if (attrNotNull) {
    std::string val = toLower(attrNotNull.value());
    if (val != "true" && val != "false") {
      insertToJson(
          errors, nodeName,
          fmt::format(
              "Expected 'true' or 'false' for notNull option, received '{}'",
              val));
    }
  }

  // Validate uuid option
  pugi::xml_attribute attrUuid = node.attribute("uuid");
  if (attrUuid) {
    std::string val = attrUuid.value();
    if (!isValidUuid(val)) {
      insertToJson(errors, nodeName,
                   fmt::format("Uuid value '{}' is not valid", val));
    }
  }
}

} // namespace validation_api
