#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <spdlog/spdlog.h>
#include <validation-api/Logger.hpp>
#include "validation-api/ConfigWatcher.hpp"

int main(int argc, char *argv[])
{
  // check for command line arguments
  if (argc > 1)
  {
    for (int i = 1; i < argc; i++)
    {
      std::string arg = argv[i];
      std::transform(arg.begin(), arg.end(), arg.begin(), ::tolower);
      if (arg == "example")
      {
        if (!boost::filesystem::exists("./configs"))
        {
          boost::filesystem::create_directory("./configs");
        }
        std::cout << "Example argument detected" << std::endl;
      }
    }
  }

  if (!validation_api::setup_logger())
  {
    std::cerr << "Logger setup failed" << std::endl;
    return 1;
  };
  auto logger = spdlog::get("Logger");

  boost::asio::io_context io_context;

  validation_api::ConfigWatcher watcher(io_context, "./configs", [](const std::string &path, const std::string &action)
                                        { std::cout << "File " << path << " was " << action << std::endl; });

  watcher.run();
  io_context.run();

  std::cout << "Hello world" << std::endl;
  return 0;
}
