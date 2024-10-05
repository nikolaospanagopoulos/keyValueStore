#pragma once
#include <cstddef>
#include <functional>
#include <mutex>
#include <shared_mutex>
#include <vector>
template <typename KeyType, typename ValueType> class UnorderedMap {
public:
  UnorderedMap(size_t initialCapacity = 8)
      : capacity(initialCapacity), size(0) {
    // make capacity number of buckets
    table.resize(capacity, {false, KeyType(), ValueType()});
  }
  void insert(const KeyType &key, const ValueType &value) {
    std::unique_lock<std::shared_mutex> lock(mutex);
    if (loadFactor() >= 0.7) {
      resize();
    }
    // the computed index will be between 0 and capacity - 1
    size_t index = hash(key) % capacity;
    while (table.at(index).occupied && table.at(index).key != key) {
      index = (index + 1) % capacity;
    }
    if (!table.at(index).occupied) {
      size++;
    }
    table[index] = {true, key, value};
  }
  void get(const KeyType &key, ValueType **value) {
    std::shared_lock<std::shared_mutex> lock(mutex);
    size_t index = hash(key) % capacity;
    // remember to avoid infinite loop
    size_t startIndex = index;

    while (table.at(index).occupied) {
      if (table.at(index).key == key) {
        *value = &(table.at(index).value);
        break;
      }
      index = (index + 1) % capacity;
      if (index == startIndex) {
        break;
      }
    }
  }

  bool contains(const KeyType &key) {
    ValueType *value = nullptr;
    get(key, &value);
    if (value) {
      return true;
    }
    return false;
  }

  bool erase(const KeyType &key) {
    std::unique_lock<std::shared_mutex> lock(mutex);
    size_t index = hash(key) % capacity;
    size_t startIndex = index;
    while (table.at(index).occupied) {
      if (table.at(index).key == key) {
        table.at(index).occupied = false;
        size--;
        return true;
      }
      index = (index + 1) % capacity;
      if (index == startIndex) {
        break;
      }
    }
    return false;
  }

private:
  struct Bucket {
    bool occupied;
    KeyType key;
    ValueType value;
  };
  std::shared_mutex mutex;
  std::vector<Bucket> table;
  size_t capacity;
  size_t size;
  size_t hash(const KeyType &key) const { return std::hash<KeyType>()(key); }
  double loadFactor() const { return static_cast<double>(size) / capacity; }

  void resize() {
    size_t newCapacity = capacity * 2;
    std::vector<Bucket> newTable(newCapacity, {false, KeyType(), ValueType()});

    for (const auto &bucket : table) {
      if (bucket.occupied) {
        size_t index = hash(bucket.key) % newCapacity;
        while (newTable.at(index).occupied) {
          index = (index + 1) % newCapacity;
        }
        newTable[index] = bucket;
      }
    }
    table = std::move(newTable);
    capacity = newCapacity;
  }
};
