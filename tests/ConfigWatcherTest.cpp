#include <gtest/gtest.h>

#include <validation-api/ConfigWatcher.hpp>
#include <fstream>
#include <boost/filesystem.hpp>

TEST(FileWatcherTest, initialization_and_running)
{
  std::string path = "./configs";
  boost::filesystem::path dir(path);

  if ((boost::filesystem::exists(dir)))
  {
    (void)system("rm -rf configs");
  }
  bool dirTest = boost::filesystem::create_directory(dir);
  ASSERT_TRUE(dirTest);

  std::ofstream firstFile(path + "/test.txt");
  std::ofstream secondFile(path + "/test2.txt");
  firstFile << "creating" << std::endl;
  secondFile << "creating" << std::endl;
  (firstFile, secondFile).close();
}
