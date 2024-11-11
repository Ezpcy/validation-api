#include <boost/asio.hpp>
#include <boost/uuid.hpp>
#include <cctype>
#include <cmath>
#include <format>
#include <lib/Helpers.hpp>
#include <string>
#include <unordered_set>

#include "lib/ErrorBuilder.hpp"
#include "validation-api/ConfigService.hpp"

namespace validation_api {

void validateXmlConfig(const pugi::xml_node& node,
                       validation_api::ConfigService::Errors& errors) {
  std::unordered_set<std::string> types = {
      "string", "number", "float", "date", "uuid", "boolean", "ahv", "iban"};

  std::string typeName;

  // Check for type
  pugi::xml_attribute attrType = node.attribute("type");
  if (attrType) {
    // Validate value
    typeName = toLower(attrType.value());
    if (types.find(typeName) == types.end()) {
      errors.push_back(
          {ErrorBuilder(ErrorType::XmlConfigValueError, node.name())
               .setSecondMsg("valid type option", attrType.value())
               .build()});
    } else {
    }
  }

  // Check for length options
  pugi::xml_attribute attrMax = node.attribute("max");
  pugi::xml_attribute attrMin = node.attribute("min");
  pugi::xml_attribute attrEq = node.attribute("eq");

  std::optional<float> min;
  std::optional<float> max;
  std::optional<float> eq;

  // Check if type can have len options
  const std::unordered_set<std::string> lenCheckType = {"string", "number",
                                                        "float"};

  if (attrMax || attrMin || attrEq) {
    if (lenCheckType.find(typeName) == lenCheckType.end()) {
      errors.push_back(
          ErrorBuilder(ErrorType::XmlConfigError, node.name())
              .setSecondMsg(std::format("Type \"{}\" is not allowed to have "
                                        "length options (eq, max, min).",
                                        typeName))
              .build());
    }
  }

  // Validate len options
  if (attrMax) {
    if (attrMax.value()) {
      std::string maxVal = toLower(attrMax.value());
      max = 0;
      if (!isFloat(maxVal, max.value())) {
        errors.push_back(
            {ErrorBuilder(ErrorType::XmlConfigValueError, node.name())
                 .setSecondMsg("number for max option", attrMax.value())
                 .build()});
      }
    } else {
      errors.push_back(
          {ErrorBuilder(ErrorType::XmlConfigEmptyError, node.name())
               .setSecondMsg("max")
               .build()});
    }
  }
  if (attrMin) {
    if (attrMin.value()) {
      std::string minVal = toLower(attrMin.value());
      min = 0;
      if (!isFloat(minVal, min.value())) {
        errors.push_back(
            {ErrorBuilder(ErrorType::XmlConfigValueError, node.name())
                 .setSecondMsg("number for min option", attrMin.value())
                 .build()});
      }
    } else {
      errors.push_back(
          {ErrorBuilder(ErrorType::XmlConfigEmptyError, node.name())
               .setSecondMsg("min")
               .build()});
    }
  }
  if (attrEq) {
    if (attrEq.value()) {
      std::string eqVal = toLower(attrEq.value());
      eq = 0;

      if (!isFloat(eqVal, eq.value())) {
        errors.push_back(
            {ErrorBuilder(ErrorType::XmlConfigValueError, node.name())
                 .setSecondMsg("number for eq option", attrEq.value())
                 .build()});
      }
    } else {
      errors.push_back(
          {ErrorBuilder(ErrorType::XmlConfigEmptyError, node.name())
               .setSecondMsg("eq")
               .build()});
    }
  }

  // Check notNull option
  pugi::xml_attribute attrNotNull = node.attribute("notNull");
  if (attrNotNull) {
    std::string notNullVall = toLower(attrNotNull.value());

    if (notNullVall != "true" && notNullVall != "false") {
      errors.push_back(
          {ErrorBuilder(ErrorType::XmlConfigValueError, node.name())
               .setSecondMsg("false or true on notNull option",
                             attrNotNull.value())
               .build()});
    }
  }

  // Check Uuid option
  pugi::xml_attribute attrUuid = node.attribute("uuid");
  if (attrUuid) {
    std::string uuidVal = attrUuid.value();

    // validate uuid
    if (!isValidUuid(uuidVal)) {
      errors.push_back(
          {ErrorBuilder(ErrorType::XmlConfigValueError, node.name())
               .setSecondMsg("valid uuid", uuidVal)
               .build()});
    }
  }

  // Recursively traverse through child nodes
  for (pugi::xml_node child = node.first_child(); child;
       child = child.next_sibling()) {
    validateXmlConfig(child, errors);  // Recursion for nested elements
  }

  // Check max and min values
  if (max.has_value() || min.has_value()) {
    if (max < min) {
      errors.push_back(
          {ErrorBuilder(ErrorType::XmlConfigError, node.name())
               .setSecondMsg({std::format("Min value ({}) is not allowed to "
                                          "be greater then max value ({}).",
                                          max.value(), min.value())})
               .build()});
    }

    // Check if eq value is set
    if (eq.has_value()) {
      errors.push_back(
          ErrorBuilder(ErrorType::XmlConfigError, node.name())
              .setSecondMsg({std::format(
                  "Eq option can not be set if max or min have been set.")})
              .build());
    }
  }
}

}  // namespace validation_api
