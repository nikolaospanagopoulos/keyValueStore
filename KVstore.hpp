#pragma once
#include "UnorderedMap.hpp"
#include <string>

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

  UnorderedMap<std::string, std::string> unorderedMap;

private:
};
