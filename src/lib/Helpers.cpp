#include <algorithm>
#include <boost/uuid.hpp>
#include <cctype>
#include <cstdio>
#include <lib/Helpers.hpp>
#include <nlohmann/json.hpp>
#include <pugixml.hpp>

namespace validation_api {
// Conversion of string to lowercase
std::string toLower(std::string value) {
  std::transform(value.begin(), value.end(), value.begin(), ::tolower);
  return value;
}

}  // namespace validation_api
