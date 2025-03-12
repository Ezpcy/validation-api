#include <gtest/gtest.h>

#include <lib/ErrorBuilder.hpp>
#include <nlohmann/json.hpp>
#include <pugixml.hpp>
#include <validation-api/ConfigService.hpp>

#include "lib/Helpers.hpp"
#include "lib/Validation.hpp"

TEST(ValidationTest, Types)
{
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

bool containsSlice(const std::string &str, const std::string &slice)
{
  return str.find(slice) != std::string::npos;
}
TEST(ValidationTest, Options)
{
  using json = nlohmann::json;
  static const std::string xml = R"(
    <Options>
      <String type="string" eq="5" />
      <String1 type="string" min="5" max="8" />
      <String2 type="string" min="5" max="8" />
      <String3 type="string" notNull="true" />
      <Float notNull="true" />
      <Null type="number">
        <AllowNullIf>
          <Uuid uuid="123e4567-e89b-12d3-a456-526614174000" />
        </AllowNullIf>
      </Null>
      <Uuid type="uuid" />
      <Number0 type="number" min="4" max="8" />
      <Float0 type="float" min="4.2" max="5.2" />
      <Number1 type="number" min="4" max="5" />
      <Float1 type="float" min="4" max="5"/>     
      <Number2 type="number" eq="4" />
      <Float2 type="float" eq="4.5" />
      <Nested notNull="true">
        <Null2 type="number">
          <AllowNullIf>
            <Uuid uuid="123e4567-e89b-12d3-a456-526614174000" />
          </AllowNullIf>
        </Null2>
        <Float3 type="float" notNull="true"/>
      </Nested>
    </Options>
  )";
  static const std::string json_true = R"({
  "Options": {
    "String": "World",
    "String1": "Hello",
    "String2": "Hello",
    "String3": "t",
    "Float": 5.2,
    "Null": null,
    "Uuid": "123e4567-e89b-12d3-a456-526614174000",
    "Number0": 5,
    "Float0": 5.0,
    "Number1": 4,
    "Float1": 4.5,    
    "Number2": 4,
    "Float2": 4.5,
    "Nested": {
      "Null2": 20,
      "Float3": 20.2
    }
  }
  })";

  static const std::string json_false = R"({
  "Options": {
    "String": "Longer than expected",
    "String1": "if",
    "String2": "shrt",
    "String3": "",
    "Float": null,
    "Null": null,
    "Uuid": "123e4567-e89b-12d3-a456-526614174001",
    "Number0": 20,
    "Float0": 10.5,
    "Number1": 2,
    "Float1": 29.3,
    "Number2": 2,
    "Float2": 4.29,
    "Nested": {
      "Null2": null,
      "Float3": null
    }
  }
  })";

  pugi::xml_document doc;
  pugi::xml_parse_result resullt = doc.load_string(xml.c_str());

  json jj = json::parse(json_true);
  validation_api::ConfigService::Errors errors;

  validation_api::Validation(jj, doc, errors).run();
  std::cout << errors.dump() << '\n';
  ASSERT_EQ(errors.size(), 0);

  json jjf = json::parse(json_false);
  validation_api::ConfigService::Errors errorsf;

  validation_api::Validation(jjf, doc, errorsf).run();
  std::cout << errorsf.dump() << '\n';

  ASSERT_TRUE(containsSlice(errorsf["Float"], "CannotBeEmpty"));
  ASSERT_TRUE(containsSlice(errorsf["Float0"], "MaxError 10.5 5.2"));
  ASSERT_TRUE(containsSlice(errorsf["Float1"], "MaxError 29.3 5"));
  ASSERT_TRUE(containsSlice(errorsf["Float2"], "EqError 4.29 4.5"));
  ASSERT_TRUE(containsSlice(errorsf["Null"], "CannotBeEmpty"));
  ASSERT_TRUE(containsSlice(errorsf["String"], "EqError 20 5"));
  ASSERT_TRUE(containsSlice(errorsf["String1"], "MinError 2 5"));
  ASSERT_TRUE(containsSlice(errorsf["String3"], "CannotBeEmpty"));
  ASSERT_TRUE(containsSlice(errorsf["String3"], "CannotBeEmpty"));
  ASSERT_TRUE(containsSlice(errorsf["Null2"], "CannotBeEmpty"));
  ASSERT_TRUE(containsSlice(errorsf["Float3"], "CannotBeEmpty"));
  ASSERT_TRUE(containsSlice(errorsf["Number0"], "MaxError 20 8"));
  ASSERT_TRUE(containsSlice(errorsf["Number1"], "MinError 2 4"));
  ASSERT_TRUE(containsSlice(errorsf["Number2"], "EqError 2 4"));
}

TEST(ValidationTest, XmlParsing)
{
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

TEST(ValidationTest, ListValidation)
{
  static const std::string xml = R"(
    <Options>
      <List type="list" elementType="string" notNull="true"/>
    </Options> 
  )";

 static const std::string json_true = R"({
    "Options": {
      "List": [
        "value",
        "value2"
    ]
  }
  })";
  pugi::xml_document doc;

  pugi::xml_parse_result result = doc.load_string(xml.c_str());

  validation_api::ConfigService::Errors errors;

  pugi::xml_node node = doc.child(doc.begin()->name());

  validation_api::validateXmlConfig(node, errors);
  errors[doc.begin()->name()] = errors;

  std::cerr << errors.dump() << '\n';

  ASSERT_EQ(errors.size(), 0);

}
