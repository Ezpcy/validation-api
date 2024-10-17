#pragma once

#include <nlohmann/json.hpp>
#include <pugixml.hpp>

#include "validation-api/ConfigService.hpp"

namespace validation_api {

/**
 * @brief Traverse through the json object and validate the fields.
 * @params jsonObj
 * @details Validates the json with passed xml doc object.
 * */
void traverseAndValidate(const nlohmann::json jsonObj, const pugi::xml_node doc,
                         ConfigService::Errors &errors);

std::string toLower(std::string value);

void validate(const std::string &optName, const std::string &optValue,
              const nlohmann::json &reqValue, const std::string &fieldName,
              ConfigService::Errors &errors);
}  // namespace validation_api
