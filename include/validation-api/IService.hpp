#pragma once

namespace validation_api {

/**
 * @brief Interface for all services.
 *
 * This interface defines the methods that all services must implement. The
 * methods are used to setup, run, and stop the service. The interface also
 * defines a method to retrieve a map of errors that the service has
 * encountered.
 */
class IService {
 public:
  /**
   * @brief Virtual destructor to ensure proper cleanup.
   */
  virtual ~IService() = default;

  /**
   * @brief Setup method to initialize the service.
   * @return true if setup was successful, false otherwise.
   */
  virtual bool setup() = 0;

  /**
   * @brief Run method to start the service.
   */
  virtual void run() = 0;

  /**
   * @brief Stop method to stop the service.
   */
  virtual void stop() = 0;
};

}  // namespace validation_api
