#include "KVstore.hpp"
#include <iostream>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
// Function to process SET command
std::string KVstore::handleSetCommand(const std::vector<std::string> &args) {
  if (args.size() < 3) {
    return "Error: SET requires a key and a value\n";
  }

  unorderedMap.insert(args[1], args[2]);
  return "OK\n";
}

std::string KVstore::rightPushVector(const std::vector<std::string> &args) {
  if (args.size() < 3) {
    return "Error: RPUSH requires a key and a value\n";
  }

  bool exists = unorderedMapOfVectors.contains(args[1]);
  std::vector<std::string> *foundVector = nullptr;
  unorderedMapOfVectors.get(args[1], &foundVector);

  if (exists) {
    {
      std::unique_lock<std::mutex> lock(vector_mutex);
      (*foundVector).push_back(args[2]);
    }

  } else {
    {
      std::unique_lock<std::mutex> lock(vector_mutex);
      std::vector<std::string> created;
      created.push_back(args[2]);
      lock.unlock();
      unorderedMapOfVectors.insert(args[1], (created));
    }
  }

  return "OK\n";
}
std::string
KVstore::getVectorElementByIndex(const std::vector<std::string> &args) {
  if (args.size() < 3) {
    return "Error: LINDEX requires a key and a list index\n";
  }
  bool exists = unorderedMapOfVectors.contains(args[1]);
  std::vector<std::string> *foundVector = nullptr;
  unorderedMapOfVectors.get(args[1], &foundVector);
  if (!exists) {
    return "Error: list doesn't exist\n";
  }
  std::unique_lock<std::mutex> lock(vector_mutex);
  int index = std::stoi(args[2]);
  if (index < 0 || index >= ((*foundVector).size())) {
    return "Error: index out of range\n";
  }
  return (*foundVector).at(std::stoi(args[2])).c_str();
}

std::string KVstore::getVectorRangle(const std::vector<std::string> &args) {

  if (args.size() < 4) {
    return "Error: Rangle requires an identifier, a start index and an end "
           "index\n";
  }
  std::vector<std::string> *foundVector = nullptr;
  unorderedMapOfVectors.get(args[1], &foundVector);
  int startIndex = std::stoi(args[2]);
  int endIndex = std::stoi(args[3]);
  if ((startIndex < 0 || startIndex >= foundVector->size()) ||
      (endIndex < 0 || endIndex >= foundVector->size())) {
    return "Error: index out of range \n";
  }
  std::unique_lock<std::mutex> lock(vector_mutex);
  int index = std::stoi(args[2]);
  std::vector<std::string>::const_iterator itStart =
      foundVector->begin() + startIndex;
  std::vector<std::string>::const_iterator itEnd =
      foundVector->begin() + endIndex;
  std::string result =
      "Printing list elements from: " + args[2] + " to: " + args[3] + "\n";
  while (itStart != foundVector->end() && itEnd != foundVector->end() &&
         itStart <= itEnd) {
    result += (*itStart) + " ";

    itStart++;
  }

  return result + " Ok\n";
}

// Function to process GET command
std::string KVstore::handleGetCommand(const std::vector<std::string> &args) {
  if (args.size() < 2) {
    return "Error: GET requires a key\n";
  }
  std::string *it = nullptr;
  unorderedMap.get(args[1], &it);
  if (it) {
    return *it + "\n";
  } else {
    return "(nil)\n"; // Redis returns (nil) if the key doesn't exist
  }
}

std::string KVstore::handleDecCommand(const std::vector<std::string> &args) {
  if (args.size() < 2) {
    return "Error: DEC requires a key\n";
  }
  std::string *value = nullptr;
  unorderedMap.get(args[1], &value);
  int newValue = 0;

  if (value) {
    try {
      newValue = std::stoi(*value);
    } catch (std::invalid_argument &e) {
      return std::string("Error: ") + e.what();
    }
  }
  newValue -= 1;
  unorderedMap.insert(args[1], std::to_string(newValue));
  return "OK\n";
}
std::string
KVstore::handleIncrementCommand(const std::vector<std::string> &args) {
  if (args.size() < 2) {
    return "Error: INC requires a key\n";
  }

  std::string *value = nullptr;
  unorderedMap.get(args[1], &value);
  int newValue = 0;

  if (value) {
    try {

      newValue = std::stoi(*value);

    } catch (std::invalid_argument &e) {
      return std::string("Error: ") + e.what();
    }
  }
  newValue += 1;
  unorderedMap.insert(args[1], std::to_string(newValue));
  return "OK\n";
}

std::string KVstore::handleExistsCommand(const std::vector<std::string> &args) {
  if (args.size() < 2) {
    return "Error: EXISTS requires a key\n";
  }

  if (unorderedMap.contains(args[1])) {
    return "Key Exists\n";
  }
  return "Not found\n";
}

std::string KVstore::handleDelCommand(const std::vector<std::string> &args) {
  if (args.size() < 2) {
    return "Error: DEL requires a key\n";
  }
  if (unorderedMap.erase(args[1])) {
    return "OK\n";
  }
  return "Key doesn't exist\n";
}
std::string KVstore::processCommand(const std::string &command) {
  // Split the command string into parts
  std::stringstream ss(command);
  std::string item;
  std::vector<std::string> parts;

  while (ss >> item) {
    parts.push_back(item);
  }

  if (parts.empty()) {
    return "Error: Empty command\n";
  }

  // Identify the command and call the corresponding handler
  if (parts[0] == "SET") {
    return handleSetCommand(parts);
  } else if (parts[0] == "GET") {
    return handleGetCommand(parts);
  } else if (parts[0] == "DEL") {
    return handleDelCommand(parts);
  } else if (parts[0] == "EXISTS") {
    return handleExistsCommand(parts);
  } else if (parts[0] == "INC") {
    return handleIncrementCommand(parts);
  } else if (parts[0] == "DEC") {
    return handleDecCommand(parts);
  } else if (parts[0] == "RPUSH") {
    return rightPushVector(parts);
  } else if (parts[0] == "LINDEX") {
    return rightPushVector(parts);
  } else if (parts[0] == "LRANGE") {
    return getVectorRangle(parts);
  } else {
    return "Error: Unknown command\n";
  }
}
