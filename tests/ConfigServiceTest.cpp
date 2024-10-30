#include <gtest/gtest.h>

#include <cctype>
#include <nlohmann/json.hpp>
#include <pugixml.hpp>
#include <validation-api/ConfigService.hpp>

#include "lib/Helpers.hpp"

static const std::string xml = R"(
<Test>
    <Provision notNull="true">
        <ProvisionNumber type="Number" notNull="true" eqLength="14"/>
        <CostApproval type="Boolean" notNull="true" />
    </Provision>
    <TypeOfProvision type="Uuid" notNull="false" />
    <Profession type="Uuid" notNull="true" />
    <SalaryType type="Uuid" notNull="false" />
    <DateFrom type="Date" notNull="true" />
    <DateTo type="Date" notNull="true" />
    <InsuranceProductConfig notNull="true">
        <TarifNumber type="Number" notNull="true" eqLength="7" />
        <BillingCategory type="String" notNull="true" />
        <Price type="Float" notNull="true" />
        <Units type="Number" notNull="true" />
    </InsuranceProductConfig>
    <Institution type="Uuid">
        <Null>
            <TypeOfProvision Uuid="123e4567-e89b-12d3-a456-526614174000"/>
            <Profession Uuid="123e4567-e89b-12d3-a456-226614174000"/>
        </Null>
    </Institution>
    <Schule type="String" notNull="false" />
    <LehrgangsBezeichnung type="Uuid" notNull="false" />
    <Ort type="Uuid" notNull="false" />
</Test>
)";

static const std::string jsonOb = R"({
  "Test": {
    "Provision": {
      "ProvisionNumber": 12345678901234,
      "CostApproval": true
    },
    "TypeOfProvision": "123e4567-e89b-12d3-a456-526614174000",
    "Profession": "123e4567-e89b-12d3-a456-226614174000",
    "SalaryType": "123e4567-e89b-12d3-a456-426614174000",
    "DateFrom": "2020-01-01",
    "DateTo": "2020-12-31",
    "InsuranceProductConfig": {
      "TarifNumber": 1234567,
      "BillingCategory": "Test",
      "Price": 123.45,
      "Units": 3
    },
    "Institution": "",
    "Schule": "Test",
    "LehrgangsBezeichnung": "123e4567-e89b-12d3-a456-426614174000",
    "Ort": ""
  }
})";

TEST(ConfigServiceTest, CreatingConfig) {
  using json = nlohmann::json;
  validation_api::ConfigService service = validation_api::ConfigService();

  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_string(xml.c_str());
  json jj = json::parse(jsonOb);

  std::cout << doc.begin()->name() << '\n';
  std::cout << doc.name() << '\n';
  std::string key = jj.begin().key();

  std::cout << key << '\n';

  service.createConfig(key, doc);

  validation_api::ConfigService::Errors errors = service.validateConfig(jj);

  for (const auto &error : errors) {
    std::cout << error.first << error.second << '\n';
  }

  // if(!errors.empty)

  ASSERT_TRUE(result);
}
