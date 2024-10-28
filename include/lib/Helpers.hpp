#pragma once

#include <nlohmann/json.hpp>
#include <pugixml.hpp>

namespace validation_api {

/*
 * @brief Converts string to all lowercase
 * @params std::string
 */
std::string toLower(std::string value);

bool validateAhv(std::string &val);
}  // namespace validation_api
