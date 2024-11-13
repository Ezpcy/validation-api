#include <format>
#include <lib/ErrorBuilder.hpp>

namespace validation_api {
ErrorBuilder::ErrorBuilder(const ErrorType &type, const std::string &fieldName)
    : type_(type), fieldName_(fieldName) {
  switch (type_) {
  case ErrorType::Default:
    firstMsg_ = "An Error occured";
    break;
  case ErrorType::CannotBeEmpty:
    res_[fieldName_] += "RIO.CannotBeEmpty";
    break;
  case ErrorType::NotCorrectType:
    res_[fieldName_] += "RIO.NotCorrectType";
    break;
  case ErrorType::MaxError:
    res_[fieldName_] += "RIO.MaxError";
    break;
  case ErrorType::MinError:
    res_[fieldName_] += "RIO.MinError";
    break;
  case ErrorType::EqError:
    res_[fieldName_] += "RIO.EqError";
    break;
  }
}

} // namespace validation_api
