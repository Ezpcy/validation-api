#pragma once

#include <nlohmann/json.hpp>
#include <pugixml.hpp>

namespace validation_api {

nlohmann::json xmlToJson(const pugi::xml_document &doc) {
  return nlohmann::json{};
}
}  // namespace validation_api
