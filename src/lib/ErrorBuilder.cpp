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

}  // namespace validation_api
