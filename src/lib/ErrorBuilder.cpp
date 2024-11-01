#include <lib/ErrorBuilder.hpp>

namespace validation_api {
ErrorBuilder::ErrorBuilder(ErrorType type, const std::string &fieldName)
    : type_(type) {
  switch (type) {
    case ErrorType::Default:
      firstMsg_ = "An Error occured";
      break;
    case ErrorType::ValidationError:
    case ErrorType::ValidaionEmptyError:
      firstMsg_ = std::format("{}: ", fieldName);
      secondMsg_ = std::string("Field is not allowed to be empty.");
      break;
    case ErrorType::XmlConfigError:
      firstMsg_ =
          std::format("Configuration field \"{}\" is not valid: ", fieldName);
      break;
    case ErrorType::XmlConfigValueError:
      firstMsg_ =
          std::format("Configuration value \"{}\" is not valid: ", fieldName);
      break;
  }
}

ErrorBuilder &ErrorBuilder::setSecondMsg(const std::string &msgMain,
                                         const std::string &msgSec) {
  switch (type_) {
    case ErrorType::Default:
      secondMsg_ = "";
      break;
    case ErrorType::ValidationError:
      secondMsg_ =
          std::format("Expected \"{}\", received \"{}\".", msgMain, msgSec);
      break;
    case ErrorType::ValidaionEmptyError:
      break;
    case ErrorType::XmlConfigError:
    case ErrorType::XmlConfigValueError:
      secondMsg_ = msgMain;
      break;
  }
  return *this;
}

std::pair<std::string, std::string> ErrorBuilder::build() const {
  return std::pair(firstMsg_, secondMsg_);
}

}  // namespace validation_api
