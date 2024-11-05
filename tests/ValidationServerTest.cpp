#include <gtest/gtest.h>

#include <lib/Helpers.hpp>
#include <nlohmann/json_fwd.hpp>
#include <validation-api/ValidationServer.hpp>

TEST(ValidationServerTest, ErrorsToJson) {
  validation_api::ConfigService::Errors errors;
  errors.push_back({std::string("Error key"), std::string("Error value")});

  nlohmann::json jsonErr = validation_api::errorsToJson(errors);

  std::cout << jsonErr.dump() << '\n';
}
