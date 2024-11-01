#include "validation-api/ConfigWatcher.hpp"

#include <sys/inotify.h>

#include <boost/filesystem.hpp>
#include <format>
#include <fstream>
#include <iostream>
#include <semaphore>

#include "lib/Helpers.hpp"

constexpr int MAX_CONCURRENT_READS = 5;
std::counting_semaphore<MAX_CONCURRENT_READS> semaphore(MAX_CONCURRENT_READS);

namespace validation_api {
/**
 * @brief Construct a new validation api::ConfigWatcher::ConfigWatcher object
 *
 * @param io_context
 * @param path
 * @param callback
 */
ConfigWatcher::ConfigWatcher(boost::asio::io_context &io_context,
                             const std::string &path, ConfigService &service,
                             Callback callback)
    : io_context_(io_context),
      path_(path),
      service_(service),
      callback_(callback),
      fileAssocation_(),
      logger_(spdlog::get("Logger") ? spdlog::get("Logger")
                                    : spdlog::default_logger()) {
  running = true;
  watcherThread = boost::thread([this] {
    if (!setup()) {
      logger_->error("Config watcher failed to initialize inotify");
      stop();
    };
    logger_->info("Config watcher initialized");
    run();
  });
}

/**
 * @brief Destroy the validation api::ConfigWatcher::ConfigWatcher object and
 * close the inotify instance.
 *
 */
ConfigWatcher::~ConfigWatcher() { stop(); }

/**
 * @brief Reads a file
 * @details Reads and validates a file expecting a Xml configuration
 * @param file_name
 */
void ConfigWatcher::read_file(const std::string &file_name) {
  // Assemble the direct path
  std::string endPath = path_ + "/" + file_name;

  // Check if the file is empty (likely newly created)
  std::ifstream file(endPath);

  if (!file.is_open()) {
    logger_->error("Failed to open file {}.", file_name);
  } else if (file.peek() != std::ifstream::traits_type::eof()) {
    pugi::xml_document doc;

    // Convert it for usage with load_file()
    const char *cEndPath = endPath.c_str();
    pugi::xml_parse_result result = doc.load_file(cEndPath);

    if (result) {
      std::string name = doc.begin()->name();
      pugi::xml_node node = doc.child(name.c_str());
      ConfigService::Errors errors;
      validateXmlConfig(node, errors);
      if (!errors.empty()) {
        errors.push_back({std::format("Errors on configuration file: "),
                          std::format("\"{} ", file_name)});
        for (const auto &error : errors) {
          // debugging
          logger_->error("{}{}", error.first, error.second);
        }
        return;
      }
      if (service_.createConfig(name, doc)) {
        fileAssocation_[file_name] = name;
      }
    } else {
      logger_->error("Failed to parse {} to a Xml object.", file_name);
    }
  }
}

/**
 * @brief Sets up the inotify instance.
 *
 * @return true if the setup is successful
 * @return false if the setup fails
 */
bool ConfigWatcher::setup() {
  inotify_fd_ = inotify_init();
  if (inotify_fd_ < 0) {
    logger_->error("Config watcher failed to initialize inotify");
    return false;
  }

  // Read the files inside the "confgis" folder
  if (std::filesystem::exists(path_)) {
    for (const auto &entry : std::filesystem::directory_iterator(path_)) {
      const auto &path = entry.path();
      if (std::filesystem::is_regular_file(path)) {
        read_file(path.filename());
      }
    }
  }
  return true;
}

/**
 * @brief Run the inotify instance and watch for changes in the directory.
 *
 * @return * void
 */
void ConfigWatcher::run() {
  // Add watch to the directory
  watch_descriptor_ = inotify_add_watch(inotify_fd_, path_.c_str(),
                                        IN_MODIFY | IN_CREATE | IN_DELETE);
  if (watch_descriptor_ < 0) {
    logger_->error("Config watcher failed to add watch to directory: {}, {}",
                   path_, std::strerror(errno));
    return;
  }

  // Buffer for reading inotify events
  constexpr size_t EVENT_SIZE = sizeof(inotify_event);
  constexpr size_t BUF_LEN = 1024 * (EVENT_SIZE + 16);
  char buffer[BUF_LEN];

  std::unordered_map<std::string, std::chrono::steady_clock::time_point>
      event_times;
  constexpr auto DEBOUNCEinTERVAL = std::chrono::milliseconds(100);

  // Use poll to avoid blocking indefinitely
  struct pollfd fds;
  fds.fd = inotify_fd_;
  fds.events = POLLIN;

  // Loop to read inotify events
  while (running) {
    semaphore.acquire();
    int poll_result = poll(&fds, 1, 1000);  // Timeout after 1000 ms
    if (poll_result < 0) {
      logger_->error("Config watcher poll failed.");
      semaphore.release();
      return;
    } else if (poll_result == 0) {
      // Timeout - check if we should continue running
      semaphore.release();
      continue;
    }

    if (fds.revents & POLLIN) {
      int no_of_event = read(inotify_fd_, buffer, BUF_LEN);
      if (no_of_event < 0) {
        logger_->error("Config watcher failed to read inotify events.");
        semaphore.release();
        return;
      }

      int count = 0;
      while (count < no_of_event) {
        struct inotify_event *event = (struct inotify_event *)&buffer[count];
        if (event->len) {
          std::string action;
          std::string file_name = event->name;
          if (event->mask & IN_CREATE) {
            action = "created";
            read_file(file_name);
          } else if (event->mask & IN_DELETE) {
            action = "deleted";
            if (fileAssocation_.contains(file_name)) {
              std::string configName = fileAssocation_[file_name];
              service_.deleteConfig(configName);
              fileAssocation_.erase(file_name);
            }
          } else if (event->mask & IN_MODIFY) {
            action = "modified";
            read_file(file_name);
          }
          auto now = std::chrono::steady_clock::now();

          auto last_time_it = event_times.find(file_name);
          if (last_time_it != event_times.end()) {
            auto time_diff = now - last_time_it->second;
            if (time_diff < DEBOUNCEinTERVAL) {
              // Debounce - skip this event
              count += EVENT_SIZE + event->len;
              continue;
            }
          }

          callback_(file_name, action);
          event_times[file_name] = now;
        }
        count += EVENT_SIZE + event->len;
      }
    }
    semaphore.release();
  }
}

void ConfigWatcher::stop() {
  running = false;  // Set running to false to stop the loop

  // Interrupt the inotify read by closing the file descriptor
  close(inotify_fd_);

  if (watcherThread.joinable()) {
    watcherThread.join();
  }

  // Remove the watch and log the shutdown
  if (watch_descriptor_ >= 0) {
    inotify_rm_watch(inotify_fd_, watch_descriptor_);
  }

  logger_->info("Config watcher stopped.");
}

}  // namespace validation_api
