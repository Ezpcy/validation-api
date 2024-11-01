#include <gtest/gtest.h>

#include <lib/ErrorBuilder.hpp>
#include <nlohmann/json.hpp>
#include <pugixml.hpp>
#include <validation-api/ConfigService.hpp>

#include "lib/Validation.hpp"

TEST(ValidationTest, Types) {
  using json = nlohmann::json;
  static const std::string xml = R"(
    <Types>
      <String type="string" />
      <Float type="float" max="5" min="3"/>
      <Number type="number" />
      <Date type="date" />
      <Email type="email" />
      <Uuid type="uuid" />
      <Bool type="boolean" />
      <Ahv type="ahv" />
      <Iban type="iban" />
    </Types>
  )";

  static const std::string json_true = R"({
  "Types": {
    "String": "test",
    "Float": 10.2,
    "Number": 10,
    "Date": "2024-01-01", 
    "Email": "test@mail.com", 
    "Uuid": "123e4567-e89b-12d3-a456-526614174000",
    "Bool": false,
    "Ahv": "756.8681.0558.61",
    "Iban": "CH8989144827467828941"
    }
  })";

  static const std::string json_false = R"({
  "Types": {
    "String": 12,
    "Float": 2,
    "Number": 10.2,
    "Date": "202401-01", 
    "Email": "testmail.com", 
    "Uuid": "23e4567-e89b-12d3-a456-526614174000",
    "Bool": "false",
    "Ahv": "756.861.0558.61",
    "Iban": "CH898914482746782894"
    }
  })";

  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_string(xml.c_str());

  json jj = json::parse(json_true);
  json jjf = json::parse(json_false);
  validation_api::ConfigService::Errors errors;

  std::cout << "Running correct" << '\n';
  validation_api::Validation(jj, doc, errors).run();

  for (const auto &[key, value] : errors) {
    std::cout << key << value << '\n';
  }

  ASSERT_TRUE(errors.empty());
  errors.clear();

  validation_api::Validation(jjf, doc, errors).run();
  std::cout << "Running fails" << '\n';
  for (const auto &[key, value] : errors) {
    std::cout << key << value << '\n';
  }

  ASSERT_FALSE(errors.empty());
  ASSERT_EQ(errors.size(), 9);
}
