#pragma once

#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <boost/thread.hpp>
#include <iostream>
#include <unordered_map>
#include <chrono>
#include <functional>
#include <string>
#include <validation-api/IService.hpp>

namespace validation_api
{
  /**
   * @brief Class to watch for changes in a directory.
   * @details This class uses the boost::asio library to watch for changes in a
   * directory. When a change is detected, the callback function is called. Used for
   * watching changes inside a configuration directory where xml files are stored.
   */
  class ConfigWatcher : public IService
  {
  public:
    using Callback = std::function<void(const std::string &path, const std::string &action)>;

    ConfigWatcher(boost::asio::io_context &io_context, const std::string &path, Callback callback);
    ~ConfigWatcher() override;

    void read_file(const std::string &path);


  private:
    void run() override;
    bool setup() override;
    void stop() override;
    boost::asio::io_context &_io_context_;
    std::string _path_;
    Callback _callback_;
    std::atomic<bool> running;
    boost::thread watcherThread;

    // Inotify variables fd = file descriptor
    int _inotify_fd_;
    // Watch descriptor
    int _watch_descriptor_;
  };
}
