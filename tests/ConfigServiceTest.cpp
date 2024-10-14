#include <gtest/gtest.h>

#include <algorithm>
#include <nlohmann/json.hpp>
#include <pugixml.hpp>
#include <validation-api/ConfigService.hpp>

TEST(ConfigServiceTest, CreatingConfig) {
  validation_api::ConfigService configService;
  std::string name = "TestConfig";
  pugi::xml_document doc;

  std::string xml = R"a(
    <RioPartnerEntity>
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
    </RioPartnerEntity>
  )a";

  doc.load_string(xml.c_str());

  configService.createConfig(name, std::move(doc));

  auto config = configService.getConfig(name);

  ASSERT_EQ(config->getName(), name);
  ASSERT_EQ(config->getXml(), xml);
}
