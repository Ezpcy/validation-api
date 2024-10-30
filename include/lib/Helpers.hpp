#pragma once

#include <nlohmann/json.hpp>
#include <pugixml.hpp>

#include "validation-api/ConfigService.hpp"

namespace validation_api {

/**
 * @brief Converts string to all lowercase
 * @params std::string value
 */
std::string toLower(std::string value);

/**
 * @brief Validates Ahv string
 * @details Runs a checksum algorithm which compares it value with the last
 * check digit
 * @params std::string& val
 */
bool validateAhv(std::string &val);

/**
 * @brief Validates Iban string
 * @details This function validates Swiss IBAN numbers. The code was taken from
 * [here](https://docs.rs/iban_validate/latest/src/iban/base_iban.rs.html#180).
 * @params std::string &val
 */
bool validateIban(const std::string &val);

/**
 * @brief Ensures valid Xml configuration
 * @details Checks all fields and options and ensure the rules are being
 * followed
 */
void validateXmlConfig(const pugi::xml_node &node,
                       validation_api::ConfigService::Errors &erros);
}  // namespace validation_api
