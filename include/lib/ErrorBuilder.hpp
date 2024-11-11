#pragma once
#include <format>

namespace validation_api {

/**
 * @brief Set the Second Msg object
 * @param msgMain Main message
 * @param msgSec Secondary message
 * @return ErrorBuilder&
 */
enum class ErrorType {
  Default,
  ValidationError,
  ValidationEmptyError,
  XmlConfigError,
  XmlConfigValueError,
  XmlConfigEmptyError,
  JsonMissingField
};

/**
 * @brief ErrorBuilder class
 * @details This class is used to build error messages
 */
class ErrorBuilder {
 public:
  /**
   * @brief Construct a new Error Builder object
   * @param type Error type
   * @param fieldName Field name
   */
  ErrorBuilder(ErrorType type = ErrorType::Default,
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
        secondMsg_ = "";
        break;
      case ErrorType::ValidationEmptyError:
        break;
      case ErrorType::XmlConfigError:
        secondMsg_ = msgMain;
        break;
      case ErrorType::ValidationError:
      case ErrorType::XmlConfigValueError:
        secondMsg_ = std::format("Expected {}, received {}", msgMain, msgSec);
        break;
      case ErrorType::XmlConfigEmptyError:
        secondMsg_ = std::format("Option {} can not be empty", msgMain);
        break;
      case ErrorType::JsonMissingField:
        break;
    }
    return *this;
  }

  inline std::pair<std::string, std::string> build() const {
    return std::pair(firstMsg_, secondMsg_);
  }

 private:
  ErrorType type_;
  std::string firstMsg_;
  std::string secondMsg_;
};
}  // namespace validation_api
