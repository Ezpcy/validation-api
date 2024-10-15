#include <gtest/gtest.h>

#include <nlohmann/json.hpp>
#include <pugixml.hpp>
#include <validation-api/ConfigService.hpp>

static const std::string xml = R"(<RioPartner>
  <PartnerEntity>
    <Salutation type="Uuid" notNull="true"/>
    <Name type="String" notNull="true"/>
    <Surname type="String" notNull="true"/>
    <Institution type="Uuid" notNull="true"/>
    <Street type="String" notNull="true"/>
    <PostalCode type="String" notNull="true"/>
    <City type="String" notNull="true"/>
    <Gln type="Number" notNull="false"/>
    <Telephone type="String" notNull="false" minLength="10"/>
    <Mobile type="String" notNull="false" minLength="10"/>
    <Email type="String" notNull="false"/>
    <CorrespondenceLanguage type="Uuid" notNull="true"/>
  </PartnerEntity>
  <Role type="Uuid" notNull="true"/>
  <ActiveSince type="Date" notNull="true"/>
  <ActiveUntil type="Date" notNull="false"/>
</RioPartner>)";

static const std::string jsonOb = R"({
  "RioPartner": {
      "PartnerEntity": {
          "Salutation": "00000000-0000-0000-0000-000000000001",
          "Name": "string",
          "Surname": "string",
          "Institution": "00000000-0000-0000-0000-000000000001",
          "Street": "string",
          "PostalCode": "string",
          "City": "string",
          "Gln": 0,
          "Telephone": "079 823 28 19",
          "Mobile": "079 823 28 19",
          "Email": "string",
          "CorrespondenceLanguage": "00000000-0000-0000-0000-000000000001"
      },
      "Role": "00000000-0000-0000-0000-000000000001",
      "ActiveSince": "2021-02-02",
      "ActiveUntil": "2021-02-02"
  }
})";

TEST(ConfigServiceTest, CreatingConfig) {
  validation_api::ConfigService service = validation_api::ConfigService();

  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_string(xml.c_str());

  ASSERT_TRUE(result);
}
