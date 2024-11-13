#pragma once
#include <format>
#include <nlohmann/json.hpp>

namespace validation_api {

/**
 * @brief Set the Second Msg object
 * @param msgMain Main message
 * @param msgSec Secondary message
 * @return ErrorBuilder&
 */
enum class ErrorType {
  Default,
  CannotBeEmpty,
  NotCorrectType,
  MaxError,
  MinError,
  EqError,
};

/**
 * @brief ErrorBuilder class
 * @details This class is used to build error messages
 */
class ErrorBuilder {
public:
  /**
   * @brief Construct a new Error Builder object f
   * @param type Error type
   * @param fieldName Field name
   */
  ErrorBuilder(const ErrorType &type = ErrorType::Default,
               const std::string &fieldName = "");

  /**
   * @brief Set the First Msg object
   * @param msg Main message
   * @return ErrorBuilder&
   */
  inline ErrorBuilder &setSecondMsg(const std::string &msgMain = "",
                                    const std::string &msgSec = "") {
    switch (type_) {
    case ErrorType::Default:
      break;
    case ErrorType::CannotBeEmpty:
      break;
    case ErrorType::NotCorrectType:
    case ErrorType::MaxError:
    case ErrorType::MinError:
    case ErrorType::EqError:
      nlohmann::json exp;
      exp["Expected"] = msgMain;
      exp["Received"] = msgSec;
      res_[fieldName_] += exp;
      break;
    }
    return *this;
  }

  nlohmann::json build() const { return res_; }

private:
  nlohmann::json res_;
  ErrorType type_;
  std::string fieldName_;
  std::string firstMsg_;
  std::string secondMsg_;
};
} // namespace validation_api
