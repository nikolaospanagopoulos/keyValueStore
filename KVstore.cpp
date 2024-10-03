#include "KVstore.hpp"
#include <sstream>
#include <stdexcept>
#include <string>
// Function to process SET command
std::string KVstore::handleSetCommand(const std::vector<std::string> &args) {
  if (args.size() < 3) {
    return "Error: SET requires a key and a value\n";
  }

  unorderedMap.insert(args[1], args[2]);
  return "OK\n";
}

// Function to process GET command
std::string KVstore::handleGetCommand(const std::vector<std::string> &args) {
  if (args.size() < 2) {
    return "Error: GET requires a key\n";
  }
  auto it = unorderedMap.get(args[1]);
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
  auto value = unorderedMap.get(args[1]);
  int newValue = 0;

  if (value.has_value()) {
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

  auto value = unorderedMap.get(args[1]);
  int newValue = 0;

  if (value.has_value()) {
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
  } else {
    return "Error: Unknown command\n";
  }
}
