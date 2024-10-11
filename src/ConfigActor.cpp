#include <boost/fiber/all.hpp>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <unordered_map>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <string>

class ConfigActor
{
public:
  ConfigActor() : stop_flag(false), actor_fiber(&ConfigActor::run, this) {}

  ~ConfigActor()
  {
    stop();
    if (actor_fiber.joinable())
    {
      actor_fiber.join();
    }
  }

  // Sends a task/message to the actor
  void send(std::function<void()> message)
  {
    std::lock_guard<std::mutex> lock(queue_mutex);
    message_queue.push(message);
    cv.notify_one(); // Notify the fiber that a new message is available
  }

  // Stops the actor from processing messages
  void stop()
  {
    {
      std::lock_guard<std::mutex> lock(queue_mutex);
      stop_flag = true;
    }
    cv.notify_one(); // Wake up the fiber to process the stop flag
  }

  // Method to request the current configuration state
  void request_config(std::function<void(std::unordered_map<std::string, std::string>)> callback)
  {
    send([this, callback]
         { callback(configurations); });
  }

  // Method to update the configuration when an edit event is detected
  void update_config(const std::string &key, const std::string &value)
  {
    send([this, key, value]
         {
            configurations[key] = value;
            std::cout << "Configuration for key '" << key << "' updated to value: " << value << std::endl; });
  }

private:
  void run()
  {
    while (true)
    {
      std::function<void()> message;
      {
        std::unique_lock<std::mutex> lock(queue_mutex);
        // Wait until there is a message in the queue or a stop flag
        cv.wait(lock, [this]
                { return !message_queue.empty() || stop_flag; });

        if (stop_flag && message_queue.empty())
        {
          break;
        }

        message = std::move(message_queue.front());
        message_queue.pop();
      }

      // Execute the message
      if (message)
      {
        message();
      }
    }
  }

  std::unordered_map<std::string, std::string> configurations;
  std::queue<std::function<void()>> message_queue;
  std::mutex queue_mutex;
  std::condition_variable cv;
  bool stop_flag;
  boost::fibers::fiber actor_fiber;
};
