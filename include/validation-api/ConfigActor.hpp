#pragma once

#include <IService.hpp>
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

namespace validation_api
{
  class ConfigActor : public IService
  {
  public:
    using ConfigMap = std::unordered_map<std::string, std::string>;

    // Constructor
    ConfigActor();

    // Destructor
    ~ConfigActor() override;

    // Send a message/task to the actor
    void send(std::function<void()> message);

    // Method to request the current configuration state
    void request_config(std::function<void(ConfigMap)> callback);

    // Method to update the configuration when an edit event is detected
    void update_config(const std::string &key, const std::string &value);

  private:
    // Main fiber loop for processing messages
    void run() override;
    
    // Stop the actor from processing messages
    void stop() override;

    // Internal state to store configurations
    ConfigMap configurations;

    // Queue for storing messages/tasks
    std::queue<std::function<void()>> message_queue;

    // Synchronization primitives for thread-safe access
    std::mutex queue_mutex;
    std::condition_variable cv;

    // Flag to stop the actor
    bool stop_flag;

    // Fiber running the actor's main loop
    boost::fibers::fiber actor_fiber;
  };
}