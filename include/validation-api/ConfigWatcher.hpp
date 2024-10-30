#pragma once

#include <boost/asio.hpp>
#include <unordered_map>
#include <validation-api/ConfigService.hpp>
#include <validation-api/IService.hpp>

namespace validation_api {
/**
 * @brief Class to watch for changes in a directory.
 * @details This class uses the boost::asio library to watch for changes in a
 * directory. When a change is detected, the callback function is called. Used
 * for watching changes inside a configuration directory where xml files are
 * stored.
 */
class ConfigWatcher : public IService {
 public:
  using Callback =
      std::function<void(const std::string &path, const std::string &action)>;

  ConfigWatcher(boost::asio::io_context &io_context, const std::string &path,
                ConfigService &service, Callback callback);
  ~ConfigWatcher() override;

  void read_file(const std::string &file_name);

 private:
  std::unordered_map<std::string, std::string> fileAssocation_;
  Callback callback_;

  /**
   * @brief Reference to the ConfigService.
   * @details Allows for interaction with the ConfigService in cases of event.
   * @see ConfigService
   */
  ConfigService &service_;

  /**
   * @brief Start ConfigWatcher
   * @details Also callls [ConfigWatcher::setup] to setup the watcher.
   */
  void run() override;

  /**
   * @brief Setup the ConfigWatcher
   * @details This function sets up the inotify watcher and starts the watcher
   * thread.
   * @return bool Returns true if the setup was successful.
   */
  bool setup() override;

  /**
   * @brief Stop the ConfigWatcher
   * @details This function stops the watcher thread and closes the inotify
   * watcher.
   */
  void stop() override;

  /**
   * @brief io_context_ reference
   * @details Reference to the io_context_ object. The io_context_ object is
   * used to run the watcher thread. It manages asynchronous I/O operations.
   */
  boost::asio::io_context &io_context_;

  /**
   * @brief Path to the directory to watch
   * @details The path to the directory to watch for changes. The path is
   * stored as a string.
   */
  std::string path_;

  /**
   * @brief running flag
   * @details Flag to indicate if the watcher is running. The flag is stored as
   * an atomic bool.
   */
  std::atomic<bool> running;

  /**
   * @brief watcherThread
   * @details The watcher thread is used to run the watcher. The watcher thread
   * is stored as a boost::thread object.
   */
  boost::thread watcherThread;

  // Inotify variables fd = file descriptor
  /**
   * @brief inotify_fd_
   * @details The inotify file descriptor is used to watch for changes in a
   * directory. The file descriptor is stored as an int.
   */
  int inotify_fd_;

  /**
   * @brief watch_descriptor_
   * @details The watch descriptor is used to watch for changes in a directory.
   * The watch descriptor is stored as an int.
   */
  int watch_descriptor_;

  /**
   * @brief Reference to the logger object..
   */
  std::shared_ptr<spdlog::logger> logger_;
};
}  // namespace validation_api
