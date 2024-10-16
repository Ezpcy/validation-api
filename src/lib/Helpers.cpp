#include <nlohmann/json.hpp>
#include <pugixml.hpp>

#include "validation-api/ConfigService.hpp"

namespace validation_api {
using json = nlohmann::json;

void traverseAndValidate(const nlohmann::json jsonObj, pugi::xml_node doc,
                         ConfigService::Errors& errors) {
  for (json::const_iterator it = jsonObj.begin(); it != jsonObj.end(); ++it) {
    std::string jsonKey = it.key();
    std::string jsonValue = it.value().dump();
    pugi::xml_node xmlNode = doc.child(jsonKey.c_str());

    if (!xmlNode) {
      errors.push_back(
          {std::string(jsonKey), std::string(": Field can't be found.")});
      continue;
    }

    if (it->is_structured() || it->is_array() || it->is_object()) {
      traverseAndValidate(jsonObj[jsonKey], xmlNode, errors);
    } else {
      std::cout << "Key: " << jsonKey << " Value: " << jsonValue << '\n';
      for (pugi::xml_attribute_iterator attr = xmlNode.attributes_begin();
           attr != xmlNode.attributes_end(); ++attr) {
        std::cout << attr->name() << " " << attr->value() << '\n';
      }
    }
  }
}

}  // namespace validation_api
