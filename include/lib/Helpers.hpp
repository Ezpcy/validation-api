#pragma once

#include <nlohmann/json.hpp>
#include <pugixml.hpp>

namespace validation_api {

/*
 * @brief Converts string to all lowercase
 * @params std::string value
 */
std::string toLower(std::string value);

/*
 * @brief Validates Ahv string
 * @details Runs a checksum algorithm which compares it value with the last
 * check digit
 * @params std::string& val
 */
bool validateAhv(std::string &val);

/*
 * @brief Validates Iban string
 * @details This function validates Swiss IBAN numbers. The code was taken from
 * [here](https://docs.rs/iban_validate/latest/src/iban/base_iban.rs.html#180).
 * @params std::string &val
 */
bool validateIban(const std::string &val);
}  // namespace validation_api
