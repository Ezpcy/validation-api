#pragma once

#include <nlohmann/json.hpp>
#include <pugixml.hpp>

#include "validation-api/ConfigService.hpp"

namespace validation_api {

void traverseAndValidate(const nlohmann::json& jsonObj, pugi::xml_node doc,
                         ConfigService::Errors errors) {}

}  // namespace validation_api
