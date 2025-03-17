#include "lib/Helpers.hpp"
#include <filesystem>
#include <gtest/gtest.h>

#include <fstream>
#include <spdlog/spdlog.h>
#include <validation-api/Logger.hpp>

// Test the logger initialization
TEST(LoggerTest, Initialization)
{
  const char *dir = "logs";
  struct stat sb;

  if (stat(dir, &sb) == 0 && S_ISDIR(sb.st_mode))
  {
    // Directory exists remove dir 
    (void)system("rm -rf logs");
  }
  // Setup
  ASSERT_TRUE(validation_api::setup_logger());
  auto logger = spdlog::get("Logger");
  // Check if logger is not null
  ASSERT_NE(logger, nullptr);

  logger->info("Logger initialized");
  logger->error("Logger error message");
  SPDLOG_INFO("LoggerTest: Initialization");
  logger->flush();
  std::ifstream infoLog("logs/info.log");
  std::ifstream errorLog("logs/error.log");
  // Check if the info log file is created
  ASSERT_TRUE(infoLog.is_open());

  std::string info_line;
  std::getline(infoLog, info_line);
  std::string error_line;
  std::getline(errorLog, error_line);
  // Check if the log message is written to the file
  ASSERT_TRUE(info_line.find("Logger initialized") != std::string::npos);
  ASSERT_TRUE(error_line.find("Logger error message") != std::string::npos);

  errorLog.close();
  infoLog.close();
  spdlog::drop_all();

  std::string logpath = "test/logs/";
  ASSERT_TRUE(validation_api::setup_logger(logpath));

  ASSERT_TRUE(std::filesystem::exists(logpath));
}

