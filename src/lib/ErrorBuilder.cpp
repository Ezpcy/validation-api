#include <lib/ErrorBuilder.hpp>

namespace validation_api {
ErrorBuilder::ErrorBuilder(ErrorType type, const std::string &fieldName)
    : type_(type) {
  switch (type) {
    case ErrorType::Default:
      firstMsg_ = "An Error occured";
      break;
    case ErrorType::ValidationError:
      firstMsg_ = std::format("{}", fieldName);
      break;
    case ErrorType::ValidationEmptyError:
      firstMsg_ = std::format("{}", fieldName);
      secondMsg_ = std::string("Field is not allowed to be empty");
      break;
    case ErrorType::XmlConfigError:
    case ErrorType::XmlConfigValueError:
    case ErrorType::XmlConfigEmptyError:
    case ErrorType::JsonMissingField:
      firstMsg_ = std::format("{}", fieldName);
      secondMsg_ = std::string("Missing Field");
      break;
  }
}

ErrorBuilder &ErrorBuilder::setSecondMsg(const std::string &msgMain,
                                         const std::string &msgSec) {
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

std::pair<std::string, std::string> ErrorBuilder::build() const {
  return std::pair(firstMsg_, secondMsg_);
}

}  // namespace validation_api
