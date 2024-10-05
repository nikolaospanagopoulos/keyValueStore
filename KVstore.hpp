#pragma once
#include "UnorderedMap.hpp"
#include <string>
#include <vector>

#define PORT 6379
#define BUFFER_SIZE 1024

class KVstore {
public:
  std::string handleSetCommand(const std::vector<std::string> &args);
  std::string handleGetCommand(const std::vector<std::string> &args);
  std::string processCommand(const std::string &command);
  std::string handleDelCommand(const std::vector<std::string> &args);
  std::string handleExistsCommand(const std::vector<std::string> &args);
  std::string handleIncrementCommand(const std::vector<std::string> &args);
  std::string handleDecCommand(const std::vector<std::string> &args);
  std::string rightPushVector(const std::vector<std::string> &args);
  std::string getVectorElementByIndex(const std::vector<std::string> &args);

private:
  UnorderedMap<std::string, std::string> unorderedMap;
  UnorderedMap<std::string, std::vector<std::string>> unorderedMapOfVectors;
  std::mutex vector_mutex;
};
