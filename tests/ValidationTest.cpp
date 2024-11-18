#include <gtest/gtest.h>

#include <lib/ErrorBuilder.hpp>
#include <nlohmann/json.hpp>
#include <pugixml.hpp>
#include <validation-api/ConfigService.hpp>

#include "lib/Helpers.hpp"
#include "lib/Validation.hpp"

TEST(ValidationTest, Types) {
  using json = nlohmann::json;
  static const std::string xml = R"(
    <Types>
      <String type="string" />
      <Float type="float"/>
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
    "Uuid": "123e4567-e89b-12d3-a456-52661174000",
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

  std::cout << errors.dump() << '\n';

  ASSERT_TRUE(errors.empty());

  nlohmann::json errorsf;

  validation_api::Validation(jjf, doc, errorsf).run();
  std::cout << "Running fails" << '\n';

  nlohmann::json errorWrap;
  errorWrap["error"] = errorsf;
  std::cout << errorWrap.dump() << '\n';

  ASSERT_FALSE(errorsf.empty());
  ASSERT_EQ(errorsf.size(), 9);
}

TEST(ValidationTest, Options) {
  using json = nlohmann::json;
  static const std::string xml = R"(
    <Options>
      <String type="string" min="5" max="10" eq="5" />
      <Float type="float" min="5" max="10" eq="5" />
      <Number type="number" min="5" max="10" eq="5" />
    </Options>
  )";
  static const std::string json_true = R"({
  "Options": {
    "String": "test",
    "Float": 10.2,
    "Number": 10
  }
  })";

  pugi::xml_document doc;
  pugi::xml_parse_result resullt = doc.load_string(xml.c_str());

  json jj = json::parse(json_true);
  validation_api::ConfigService::Errors errors;

  validation_api::Validation(jj, doc, errors).run();

  std::cout << errors.dump() << '\n';
}

TEST(ValidationTest, XmlParsing) {
  static const std::string xml = R"(
    <Options>
      <String type="uuid" max="1" min="2" eq="3"/>
      <Test type="2" notNull="fle"/>
      <Test2 max="k" min="c" eq="f"/>
      <Test3 uuid="2" />
      <Test4>
        <Test5 type="fk" />
        <Test6 type="oiqwhe" />
      </Test4> 
    </Options>
  )";

  static const std::string xmlT = R"(
    <Options>
      <String type="uuid">
      <Test type="string" notNull="true"/>
      <Test2 type="number" max="2"/>
      <Test3 uuid="123e4567-e89b-12d3-a456-526614174000" />
      <Test4>
        <Test5 type="iban" />
        <Test6 type="ahv" />
      </Test4>
    </Options>
  )";

  pugi::xml_document doc;

  pugi::xml_parse_result result = doc.load_string(xml.c_str());

  validation_api::ConfigService::Errors errors;

  pugi::xml_node node = doc.child(doc.begin()->name());

  validation_api::validateXmlConfig(node, errors);
  errors[doc.begin()->name()] = errors;

  std::cout << errors.dump() << '\n';

  ASSERT_EQ(errors.size(), 7);

  validation_api::ConfigService::Errors errorst;
  pugi::xml_document docT;

  docT.load_string(xmlT.c_str());

  pugi::xml_node nodeT = docT.child(docT.begin()->name());

  validation_api::validateXmlConfig(nodeT, errorst);

  std::cout << errorst.dump() << '\n';

  ASSERT_EQ(errorst.size(), 0);
}
