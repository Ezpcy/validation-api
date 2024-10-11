#include "validation-api/ConfigWatcher.hpp"
#include <iostream>
#include <fstream>
#include <thread>
#include "validation-api/Logger.hpp"
#include <sys/inotify.h>
#include <unistd.h>
#include <semaphore>

constexpr int MAX_CONCURRENT_READS = 5;
std::counting_semaphore<MAX_CONCURRENT_READS> semaphore(MAX_CONCURRENT_READS);

namespace validation_api
{
  /**
   * @brief Construct a new validation api::ConfigWatcher::ConfigWatcher object
   *
   * @param io_context
   * @param path
   * @param callback
   */
  ConfigWatcher::ConfigWatcher(boost::asio::io_context &io_context, const std::string &path, Callback callback)
      : _io_context_(io_context), _path_(path), _callback_(callback)
  {
    if(!setup()) {
      auto logger = spdlog::get("Logger");
      logger->error("Config watcher failed to initialize inotify");
      stop();
    };
  }

  /**
   * @brief Destroy the validation api::ConfigWatcher::ConfigWatcher object and close the inotify instance.
   *
   */
  ConfigWatcher::~ConfigWatcher()
  {
  }

  /**
   * @brief Sets up the inotify instance.
   *
   * @return true if the setup is successful
   * @return false if the setup fails
   */
  bool ConfigWatcher::setup()
  {
    _inotify_fd_ = inotify_init();

    auto logger = spdlog::get("Logger");

    if (_inotify_fd_ < 0)
    {
      logger->error("Config watcher failed to initialize inotify");
      return false;
    }
    return true;
  }
  /**
   * @brief Run the inotify instance and watch for changes in the directory.
   *
   * @return * void
   */
  void ConfigWatcher::run()
  {
    auto logger = spdlog::get("Logger");

    // Add watch to the directory
    _watch_descriptor_ = inotify_add_watch(_inotify_fd_, _path_.c_str(), IN_MODIFY | IN_CREATE | IN_DELETE);
    if (_watch_descriptor_ < 0)
    {
      logger->error("Config watcher failed to add watch to directory: {}", _path_);
      return;
    }

    // Buffer for reading inotify events
    constexpr size_t EVENT_SIZE = sizeof(inotify_event);
    constexpr size_t BUF_LEN = 1024 * (EVENT_SIZE + 16);
    char buffer[BUF_LEN];

    // Loop to read inotify events$
    while (true)
    {

      semaphore.acquire();
      // Read 4096 bytes from inotify file descriptor into buffer

      int length = read(_inotify_fd_, buffer, BUF_LEN);
      if (length < 0)
      {
        logger->error("Config watcher failed to read inotify events");
        return;
      }

      // Debounce the events
      std::this_thread::sleep_for(std::chrono::milliseconds(1500));

      // Iterate over the buffer
      for (int i = 0; i < length;)
      {
        // Cast the buffer to an inotify_event struct
        struct inotify_event *event = (struct inotify_event *)&buffer[i];
        if (event->len)
        {
          std::string action;
          if (event->mask & IN_CREATE)
          {
            action = "created";
          }
          else if (event->mask & IN_DELETE)
          {
            action = "deleted";
          }
          else if (event->mask & IN_MODIFY)
          {
            action = "modified";
          }
          _callback_(event->name, action);
        }
        i += EVENT_SIZE + event->len;
      }
      semaphore.release();
    }
  }

  void ConfigWatcher::stop()
  {
    close(_inotify_fd_);
    auto logger = spdlog::get("Logger");
    logger->info("Config watcher stopped");
  }

  void ConfigWatcher::read_file(const std::string &path)
  {
  }

}
