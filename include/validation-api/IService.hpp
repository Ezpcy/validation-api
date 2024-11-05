#pragma once

namespace validation_api {

/**
 * @brief Interface for all services.
 *
 * This interface defines the methods that all services must implement. The
 * methods are used to setup, run, and stop the service.
 */
class IService {
 public:
  /**
   * @brief Virtual destructor to ensure proper cleanup.
   */
  virtual ~IService() = default;

  /**
   * @brief Run method to start the service.
   */
  virtual void run() = 0;
  /**
   * @brief Setup method to initialize the service.
   * @return true if setup was successful, false otherwise.
   */
  virtual bool setup() = 0;
  /**
   * @brief Stop method to stop the service.
   */
  virtual void stop() = 0;
};

}  // namespace validation_api
