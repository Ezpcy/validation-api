#include <lib/Helpers.hpp>
#include <lib/Validation.hpp>

#include "lib/ErrorBuilder.hpp"

namespace validation_api
{
  using json = nlohmann::json;

  // Map to decide which type needs to be validated
  std::unordered_map<std::string, int> getType = {
      {"string", 1}, {"float", 2}, {"number", 3}, {"date", 4}, {"email", 5}, {"uuid", 6}, {"boolean", 7}, {"ahv", 8}, {"iban", 9}};

  Validation::Validation(const json &jsonObj, const pugi::xml_node &doc,
                         ConfigService::Errors &errors)
      : request_(jsonObj), config_(doc), errors_(errors), nullOptions_(),
        logger_(spdlog::get("Logger") ? spdlog::get("Logger")
                                      : spdlog::default_logger())
  {
    // Extract nulloptions from xml
    extractNullOptions(config_);
  };

  Validation::~Validation() = default;

  void Validation::run()
  {
    fillRequestList(request_);

    traverseAndValidate(config_);

    if (!requestList_.empty())
    {
      for (const auto &field : requestList_)
      {
        errorBuilder(errors_, ErrorType::RequestFieldNotFound, field);
      }
    }
  }

  // Validate a field
  void Validation::validate(const pugi::xml_node &node, const json &reqValue,
                            const std::string &fieldName)
  {
    try
    {
      // Assign a bool for the "notNull" option
      pugi::xml_attribute configNullOpt = node.attribute("notNull");
      // Optional floats for max, min, and eq
      std::optional<float> max;
      std::optional<float> min;
      std::optional<float> eq;

      // Check if each attribute exists before assigning
      if (node.attribute("max"))
      {
        max = node.attribute("max").as_float();
      }
      if (node.attribute("min"))
      {
        min = node.attribute("min").as_float();
      }
      if (node.attribute("eq"))
      {
        eq = node.attribute("eq").as_float();
      }
      bool canBeEmpty;
      std::string notNullVal = toLower(std::string(configNullOpt.value()));

      // Set the bool accordingly to the attribute
      if (notNullVal == "true")
      {
        canBeEmpty = false;
      }
      else if (notNullVal == "false")
      {
        canBeEmpty = true;
        // Default
      }
      else
      {
        canBeEmpty = false;
      }

      // Check for nullOptions
      if (nullOptions_.find(fieldName) != nullOptions_.end())
      {
        auto it = nullOptions_.find(fieldName)->second;
        // Iterate over the null options
        for (const auto &[key, value] : it)
        {

          // Check all fields
          if (checkNullOptionsVec(value))
          {
            canBeEmpty = true;
            break;
          }
        }
      }

      // Check if field is empty
      if (isNullOrEmpty(reqValue))
      {
        if (!canBeEmpty)
        {
          errorBuilder(errors_, ErrorType::CannotBeEmpty, fieldName);
        }
        return;
      }

      std::string configTypeOpt = toLower(node.attribute("type").value());
      // Validate type field
      auto it = getType.find(configTypeOpt);
      if (it != getType.end())
      {
        switch (it->second)
        {
        case 1:
          // Check if the value is a string
          if (!reqValue.is_string())
          {
            errorBuilder(errors_, ErrorType::NotCorrectType, fieldName, it->first,
                         reqValue.type_name());
            break;
          }
          if (max.has_value() || min.has_value() || eq.has_value())
          {
            // Get the string and its length
            std::string valueStr = reqValue;
            size_t length = valueStr.length();

            Validation::validateConstraints(fieldName, length, max, min, eq);
          }
          break;
        case 2:
          // Check if the value is a float
          if (!reqValue.is_number_float())
          {
            std::string val;
            if (reqValue.is_string())
            {
              val = reqValue.get<std::string>();
            }
            else if (reqValue.is_number_integer())
            {
              val = std::to_string(reqValue.get<int>());
            }
            else
            {
              val = "";
            }
            errorBuilder(errors_, ErrorType::NotCorrectType, fieldName, "float",
                         val);

            break;
          }
          if (max.has_value() || min.has_value() || eq.has_value())
          {
            float value = reqValue;

            Validation::validateConstraints(fieldName, value, max, min, eq);
          }
          break;
        case 3:
          // Check if the value is a number
          if (!reqValue.is_number_integer())
          {
            std::string val;
            if (reqValue.is_string())
            {
              val = reqValue.get<std::string>();
            }
            else if (reqValue.is_number_float())
            {
              val = std::format("{}", reqValue.get<double>());
            }
            else
            {
              val = "";
            }
            errorBuilder(errors_, ErrorType::NotCorrectType, fieldName, "integer",
                         val);
            break;
          }
          if (max.has_value() || min.has_value() || eq.has_value())
          {
            float value = reqValue;
            Validation::validateConstraints(fieldName, value, max, min, eq);
          }
          break;
        case 4:
        {
          // Check if the value is a date
          // We can skip special string types when it's empty and allowed to be
          if (canBeEmpty && reqValue.get<std::string>() == "")
          {
            break;
          }
          else
          {
            const std::regex date_regex(
                R"(^\d{4}-(0[1-9]|1[0-2])-(0[1-9]|[12]\d|3[01])$)");
            std::string date = reqValue.get<std::string>();
            if (!std::regex_match(date, date_regex))
            {
              errorBuilder(errors_, ErrorType::NotCorrectType, fieldName,
                           it->first, date);
            }
          }
          break;
        case 5:
        {
          // Check if the value is an email
          const std::regex email_regex(R"(^[a-zA-Z0-9_.+-]+@[a-zA-Z0-9-.]+$)");
          std::string email = reqValue.get<std::string>();
          if (!std::regex_match(email, email_regex))
          {
            errorBuilder(errors_, ErrorType::NotCorrectType, fieldName, it->first,
                         reqValue);
          }
        }
        break;
        case 6:
        {
          // Check if the value is a uuid
          if (canBeEmpty && reqValue.get<std::string>() == "" ||
              reqValue.is_null() || reqValue.empty())
          {
            break;
          }
          if (!isValidUuid(reqValue.get<std::string>()))
          {
            errorBuilder(errors_, ErrorType::NotCorrectType, fieldName, it->first,
                         reqValue);
          }
          break;
        case 7:
        {
          // Check if the value is a boolean
          if (!reqValue.is_boolean())
          {
            errorBuilder(errors_, ErrorType::NotCorrectType, fieldName, it->first,
                         reqValue.type_name());
          }
        }
        break;
        case 8:
        {
          // Check if the value is a valid Ahv
          std::string val = reqValue.get<std::string>();
          if (canBeEmpty && val == "" || reqValue.is_null() || reqValue.empty())
          {
            break;
          }
          if (!validateAhv(val))
          {
            errorBuilder(errors_, ErrorType::NotCorrectType, fieldName, it->first,
                         reqValue);
          }
        }
        break;
        case 9:
        {
          // Check if the value is a valid Iban
          std::string val = reqValue.get<std::string>();
          if (canBeEmpty && val == "" || reqValue.is_null() || reqValue.empty())
          {
            break;
          }
          if (!validateIban(val))
          {
            errorBuilder(errors_, ErrorType::NotCorrectType, fieldName, it->first,
                         reqValue);
          }
        }
        break;
        default:
          break;
        }
        }
        }
      }
    }
    catch (const std::exception &e)
    {
      errors_[fieldName] += std::string(e.what());
    }
  }

  // Recursive function to traverse and validate the request
  void Validation::traverseAndValidate(const pugi::xml_node &node)
  {
    for (pugi::xml_node field : node.children())
    {
      // Convert to string
      std::string nodeName = field.name();
      // Check if it is a Null field
      if (nodeName == "AllowNullIf")
      {
        continue;
      }

      // Check if json request has the field
      std::pair<bool, json> jsonField = findJsonField(request_, nodeName);

      if (jsonField.first)
      {
        validate(field, jsonField.second, nodeName);
        requestList_.erase(nodeName);
      }
      else
      {
        errorBuilder(errors_, ErrorType::MissingField, nodeName);
        continue;
      }

      traverseAndValidate(field);
    }
  }
} // namespace validation_api
