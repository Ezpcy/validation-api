#include <gtest/gtest.h>

#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <chrono>
#include <condition_variable>
#include <fstream>
#include <mutex>
#include <validation-api/ConfigWatcher.hpp>

#include "validation-api/ConfigService.hpp"

class FileWatcherTest : public ::testing::Test {
protected:
  boost::asio::io_context io_context;
  std::optional<
      boost::asio::executor_work_guard<boost::asio::io_context::executor_type>>
      work_guard;
  std::thread io_thread;
  std::mutex mtx;
  std::condition_variable cv;
  std::string path;
  std::string action;
  bool event_received = false;
  validation_api::ConfigService service;

  void SetUp() override {
    work_guard.emplace(boost::asio::make_work_guard(io_context));
    io_thread = std::thread([this] { io_context.run(); });
    path.clear();
    action.clear();
    event_received = false;
  }

  void TearDown() override {
    work_guard.reset(); // Allow the io_context to exit when no more work
    io_context.stop();  // Ensure io_context stops if it's still running
    if (io_thread.joinable()) {
      io_thread.join();
    }
  }

  validation_api::ConfigWatcher initializeWatcher() {
    return validation_api::ConfigWatcher(
        io_context, "./templates", service,
        [this](const std::string &p, const std::string &a) {
          std::lock_guard<std::mutex> lock(mtx);
          path = p;
          action = a;
          event_received = true;
          cv.notify_one();
        });
  }

  bool waitForEvent(
      std::chrono::milliseconds timeout = std::chrono::milliseconds(1000)) {
    std::unique_lock<std::mutex> lock(mtx);
    return cv.wait_for(lock, timeout, [this] { return event_received; });
  }
};

TEST_F(FileWatcherTest, InitializationAndRunningAndStopping) {
  boost::filesystem::path dir("./templates");
  if (boost::filesystem::exists(dir)) {
    (void)system("rm -rf ./templates");
  }
  bool dirTest = boost::filesystem::create_directory(dir);
  ASSERT_TRUE(dirTest);

  validation_api::ConfigWatcher watcher = initializeWatcher();

  // Wait briefly to ensure the watcher starts
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  // Test initial state
  ASSERT_EQ(path, "");
  ASSERT_EQ(action, "");

  // Wait for a brief moment to confirm it has stopped (should not receive any
  // event)
  ASSERT_FALSE(event_received);
}

TEST_F(FileWatcherTest, CreatingFile) {
  validation_api::ConfigWatcher watcher = initializeWatcher();

  {
    std::unique_lock<std::mutex> lock(mtx);
    (void)system("touch ./templates/test.xml");
    if (!cv.wait_for(lock, std::chrono::seconds(2),
                     [this] { return event_received; })) {
      FAIL() << "Timeout waiting for file creation event";
    }
  }

  ASSERT_EQ(path, "test.xml");
  ASSERT_EQ(action, "created");
}

TEST_F(FileWatcherTest, ModifyingFile) {
  validation_api::ConfigWatcher watcher = initializeWatcher();

  {
    std::unique_lock<std::mutex> lock(mtx);
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));  

    std::ofstream file("templates/test.xml");
    std::string content = R"(
      <Types>
        <String type="string" />
        <Float type="float" />
        <Number type="number" />
        <Date type="date" />
        <Email type="email" />
        <Uuid type="uuid" />
        <Bool type="boolean" />
        <Ahv type="ahv" />
        <Iban type="iban" />
      </Types>
    )";
    file.write(content.c_str(), content.size());
    file.close();
    if (!cv.wait_for(lock, std::chrono::seconds(2),
                     [this] { return event_received; })) {
      FAIL() << "Timeout waiting for file modification event";
    }
  }

  ASSERT_EQ(path, "test.xml");
  ASSERT_EQ(action, "modified");
}

TEST_F(FileWatcherTest, DeletingFile) {
  validation_api::ConfigWatcher watcher = initializeWatcher();

  {
    std::unique_lock<std::mutex> lock(mtx);
    (void)system("rm ./templates/test.xml");
    if (!cv.wait_for(lock, std::chrono::seconds(2),
                     [this] { return event_received; })) {
      FAIL() << "Timeout waiting for file deletion event";
    }
  }

  ASSERT_EQ(path, "test.xml");
  ASSERT_EQ(action, "deleted");
}
