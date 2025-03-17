#pragma once
#include <fmt/core.h>
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

inline std::string ErrorTypeToString(ErrorType errorType, bool isList = false) {
  switch (errorType) {

  default:
  case ErrorType::Default:
    return "Default";
  case ErrorType::ConfigurationNotFound:
    return "ConfigurationNotFound";
  case ErrorType::ChildNodeNotFound:
    return "ChildNodeNotFound";
  case ErrorType::RequestFieldNotFound:
    return "RequestFieldNotFound";
  case ErrorType::MissingField:
    return "MissingField";
  case ErrorType::CannotBeEmpty:
    return "CannotBeEmpty";
  case ErrorType::NotCorrectType:
    if (isList) {
      return "ElementNotCorrectType";
    } else {
      return "NotCorrectType";
    }
  case ErrorType::MaxError:
    if (isList) {
      return "MaxElementError";
    } else {
      return "MaxError";
    }
  case ErrorType::MinError:
    if (isList) {
      return "MinElementError";
    } else {
      return "MinError";
    }
  case ErrorType::EqError:
    if (isList) {
      return "EqElementError";
    } else {
      return "EqError";
    }
  }
}

/**
 * @brief Error builder
 * @param type Error type
 * @param fieldName Field name
 * @param exp Expected value
 * @param rec Received value
 *
 * Function to build a json error message which can occur at the validation
 * process.
 */
inline void errorBuilder(nlohmann::json &res, const ErrorType &type,
                         const std::string &fieldName,
                         const std::string &exp = "",
                         const std::string &rec = "", bool isList = false) {

    std::string toFormat = fmt::format("RIO.{} {} {}", ErrorTypeToString(type, isList), rec, exp);
    toFormat.erase(toFormat.find_last_not_of(" \n\r\t") + 1);
  res[fieldName] = toFormat;
}
} // namespace validation_api
