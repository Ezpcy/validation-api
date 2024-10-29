#include <cmath>
#include <iostream>
#include <lib/Helpers.hpp>
#include <regex>
#include <string>

namespace validation_api {
// Conversion of string to lowercase
std::string toLower(std::string value) {
  std::transform(value.begin(), value.end(), value.begin(), ::tolower);
  return value;
}

bool validateAhv(std::string& val) {
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

  // Cast the last digit as an integer
  int lastDigit = lastChar - '0';

  // Remove the last digit from the original string
  val.pop_back();

  // Remove dots from the string
  val.erase(std::remove(val.begin(), val.end(), '.'), val.end());

  // Reverse the string for the next part of the calculation
  std::reverse(val.begin(), val.end());

  // Calculate the sum by multiplying every second digit by 3
  int sum = 0;
  for (std::size_t i = 0; i < val.size(); ++i) {
    char c = val[i];

    // Make sure we only work with digits
    if (!std::isdigit(c)) {
      return false;
    }

    // Convert char to int
    int digit = c - '0';

    // Multiply every second digit by 3
    int add = (i % 2 == 0) ? digit * 3 : digit;

    sum += add;
  }

  // Calculate the next ten by rounding up the sum divided by 10
  int nextTen = static_cast<int>(std::ceil(sum / 10.0) * 10);

  // Calculate the checksum by subtracting the sum from the next ten
  int checkSum = nextTen - sum;

  // Compare the checksum with the original last digit
  if (checkSum != lastDigit) {
    return false;
  }

  return true;
}

bool validateIban(const std::string& val) {
  // Remove all whitespaces from the IBAN
  std::string address;
  for (char ch : val) {
    if (!std::isspace(static_cast<unsigned char>(ch))) {
      address += ch;
    }
  }

  // Check if the IBAN is 21 characters long
  if (address.size() != 21) {
    return false;
  }

  // Check if the IBAN starts with "CH"
  if (address.substr(0, 2) != "CH") {
    return false;
  }

  // Move the first four characters to the back
  std::string rearranged = address.substr(4) + address.substr(0, 4);

  // Convert the rearranged string into a numerical form (digits where letters
  // A-Z are replaced by 10-35)
  std::string numerical;
  for (char ch : rearranged) {
    if (std::isdigit(ch)) {
      numerical += ch;
    } else if (std::isalpha(ch)) {
      // Convert letters 'A'-'Z' to numbers '10'-'35'
      numerical += std::to_string(ch - 'A' + 10);
    } else {
      return false;  // Invalid character
    }
  }

  // Calculate the checksum using modulo 97
  const int MOD = 97;
  unsigned long long checksum = 0;
  for (char digit : numerical) {
    // Multiply the current checksum by 10 and add the next digit
    checksum = (checksum * 10 + (digit - '0')) % MOD;
  }

  // The IBAN is valid if the result of modulo 97 is 1
  if (checksum != 1) {
    return false;
  }

  // Additionally check that the digits between index 2 and 4 are not "00" or
  // "01"
  std::string checkDigits = address.substr(2, 2);
  if (checkDigits == "00" || checkDigits == "01") {
    return false;
  }

  return true;
}

}  // namespace validation_api
