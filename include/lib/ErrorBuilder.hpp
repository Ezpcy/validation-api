#pragma once
#include <format>
#include <iostream>

namespace validation_api {

enum class ErrorType {
  Default,
  ValidationError,
  ValidaionEmptyError,
  XmlConfigError,
  XmlConfigValueError

};

class ErrorBuilder {
 public:
  ErrorBuilder(ErrorType type = ErrorType::Default,
               const std::string &fieldName = "");

  ErrorBuilder &setSecondMsg(const std::string &msgMain = "",
                             const std::string &msgSec = "");

  std::pair<std::string, std::string> build() const;

 private:
  ErrorType type_;
  std::string firstMsg_;
  std::string secondMsg_;
};
}  // namespace validation_api
