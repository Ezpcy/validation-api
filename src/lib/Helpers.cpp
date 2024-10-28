#include <cmath>
#include <lib/Helpers.hpp>
#include <regex>
#include <string>

namespace validation_api {
// Conversion of string to lowercase
std::string toLower(std::string value) {
  std::transform(value.begin(), value.end(), value.begin(), ::tolower);
  return value;
}

bool validateAhv(std::string &val) {
  // AHV regex
  const std::regex ahv_regex(R"(^[7][5][6][.][\d]{4}[.][\d]{4}[.][\d]{2}$)");

  // Match the val with the regex
  if (!std::regex_match(val, ahv_regex)) {
    return false;
  }

  // Get the last char of the parameter
  char lastChar = val.back();

  // Check if it's a digit
  if (!std::isdigit(lastChar)) {
    return false;
  }

  // Cast as int
  int lastDigit = lastChar - '0';

  val.pop_back();
  val.erase(std::remove(val.begin(), val.end(), '.'), val.end());

  int sum = 0;

  // Calculate for each digit the sum by multiplying every second digit 3
  for (std::size_t i = 0; i < val.size(); ++i) {
    char c = val[i];

    if (!std::isdigit(c)) {
      return false;
    }

    // Convert char to intger
    int digit = c - '0';

    // Multiply ever second digit by 3
    int add = (i % 2 == 0) ? digit * 3 : digit;

    sum += add;
  }

  // Calculate the next ten by rounding up the sum divided by 10 and multiply by
  // 10
  int nextTen = (std::ceil(sum / 10.0) * 10);
  int checkSum = nextTen - sum;
  if (checkSum != lastDigit) {
    return false;
  } else {
    return true;
  }
}

}  // namespace validation_api
