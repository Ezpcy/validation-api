#include "validation-api/ConfigWatcher.hpp"

#include <spdlog/spdlog.h>
#include <sys/inotify.h>
#include <unistd.h>

#include <memory>
#include <semaphore>

#include "validation-api/ConfigService.hpp"

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
                             const std::string &path, Callback callback)
    : io_context_(io_context),
      path_(path),
      callback_(callback),
      service_(std::make_unique<ConfigService>()) {
  running = true;
  watcherThread = boost::thread([this] {
    auto logger = spdlog::get("Logger");
    if (!setup()) {
      logger->error("Config watcher failed to initialize inotify");
      stop();
    };
    logger->info("Config watcher initialized");
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
 * @brief Sets up the inotify instance.
 *
 * @return true if the setup is successful
 * @return false if the setup fails
 */
bool ConfigWatcher::setup() {
  inotify_fd_ = inotify_init();

  auto logger = spdlog::get("Logger");

  if (inotify_fd_ < 0) {
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
void ConfigWatcher::run() {
  auto logger = spdlog::get("Logger");

  // Add watch to the directory
  watch_descriptor_ = inotify_add_watch(inotify_fd_, path_.c_str(),
                                        IN_MODIFY | IN_CREATE | IN_DELETE);
  if (watch_descriptor_ < 0) {
    logger->error("Config watcher failed to add watch to directory: {}, {}",
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
      logger->error("Config watcher poll failed");
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
        logger->error("Config watcher failed to read inotify events");
        semaphore.release();
        return;
      }

      int count = 0;
      while (count < no_of_event) {
        struct inotify_event *event = (struct inotify_event *)&buffer[count];
        if (event->len) {
          std::string action;
          // TODO: Create boost threads
          if (event->mask & IN_CREATE) {
            action = "created";
          } else if (event->mask & IN_DELETE) {
            action = "deleted";
          } else if (event->mask & IN_MODIFY) {
            action = "modified";
          }

          std::string file_name = event->name;
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

          event_times[file_name] = now;
          callback_(file_name, action);
        }
        count += EVENT_SIZE + event->len;
      }
    }
    semaphore.release();
  }
}

void ConfigWatcher::stop() {
  auto logger = spdlog::get("Logger");
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

  logger->info("Config watcher stopped");
}

void ConfigWatcher::read_file(const std::string &path) {}

}  // namespace validation_api
