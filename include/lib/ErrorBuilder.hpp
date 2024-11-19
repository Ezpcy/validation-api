#pragma once
#include <format>
#include <nlohmann/json.hpp>
#include <string>

namespace validation_api {

/**
 * @brief Enum to map error message
 */
enum class ErrorType {
  Default,
  ConfigurationNotFound,
  ChildNodeNotFound,
  RequestFieldNotFound,
  MissingField,
  CannotBeEmpty,
  NotCorrectType,
  MaxError,
  MinError,
  EqError,
};

/**
 * @brief Error builder
 * @param type Error type
 * @param fieldName Field name
 * @param exp Expected value
 * @param rec Received value
 *
 * Function to build a json error message which can occur at the validation
 * process .
 */
inline void errorBuilder(nlohmann::json &res, const ErrorType &type,
                         const std::string &fieldName,
                         const std::string &exp = "",
                         const std::string &rec = "") {

  switch (type) {
  case ErrorType::Default:
    res[fieldName] = "Unknown";
    break;
  case ErrorType::ConfigurationNotFound:
    res[fieldName] = "ConfigurationNotFound";
    break;
  case ErrorType::ChildNodeNotFound:
    res[fieldName] = "ChildNodeNotFound";
    break;
  case ErrorType::RequestFieldNotFound:
    res[fieldName] = "RequestFieldNotFound";
    break;
  case ErrorType::MissingField:
    res[fieldName] = "MissingField";
    break;
  case ErrorType::CannotBeEmpty:
    res[fieldName] = "CannotBeEmpty";
    break;
  case ErrorType::NotCorrectType:
    res[fieldName] = std::format("NotCorrectType {} {}", rec, exp);
    break;
  case ErrorType::MaxError:
    res[fieldName] = std::format("MaxError {} {}", rec, exp);
    break;
  case ErrorType::MinError:
    res[fieldName] = std::format("MinError {} {}", rec, exp);
    break;
  case ErrorType::EqError:
    res[fieldName] = std::format("EqError {} {}", rec, exp);
    break;
  }
}

} // namespace validation_api
