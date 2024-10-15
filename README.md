# C++ Key-Value Store Server (Redis-like)

A lightweight, multi-threaded in-memory key-value store built with C++. This project is inspired by **Redis** and implements several basic commands such as `SET`, `GET`, `DEL`, and `EXISTS`. The server handles multiple clients concurrently using a custom thread pool, while the key-value store is implemented using a custom-built hash map.

## Features

- **Multi-threaded Server**: Uses a thread pool to handle multiple client connections simultaneously.
- **In-memory Key-Value Store**: Data is stored in memory using a custom hash map (`UnorderedMap`).
- **Thread-safe Operations**: Ensures safe concurrent access to the key-value store using C++ `shared_mutex`.
- **Custom Command Support**: Implements basic Redis-like commands such as `SET`, `GET`, `DEL`, `EXISTS`, and more.
- **Expandable Design**: Easily add more commands or functionalities (e.g., `APPEND`, `EXPIRE`).

## Commands Implemented

- **SET key value**: Stores the specified `key` with the given `value`.
- **GET key**: Retrieves the value associated with the specified `key`.
- **DEL key**: Deletes the specified `key` and its associated value.
- **EXISTS key**: Checks if the specified `key` exists in the store.
- **INC key**: Increments the integer value of the specified `key`.
- **DEC key**: Decrements the integer value of the specified `key`.
- **RPUSH**: Pushes into a list `key` the specified value `value` (if list doesn't exist, will be created).
- **LINDEX**: Retrieves the value of the list specified `key` at specified `index`.
- **LRANGE**: Retrieves the values of the list specified `key` at specified `range`.
- **LPOP**: Removes the first element of the list specified by `key`.

## How It Works

The server is designed to handle multiple client connections using a thread pool. The main thread listens for incoming connections, while worker threads handle each client connection, processing commands sent by the clients and interacting with the key-value store.

The key-value store itself is implemented using a custom `UnorderedMap` that works similarly to a hash map. It supports basic operations like insertion, retrieval, and deletion, and it resizes dynamically as more entries are added.

## Architecture

1. **Thread Pool**:

   - The thread pool manages a fixed number of worker threads, which process incoming client requests concurrently.
   - Each worker thread is responsible for handling the connection, processing commands, and returning responses.

2. **UnorderedMap**:
   - The key-value store is backed by a custom-built `UnorderedMap`, which is a hash map-like data structure that supports dynamic resizing and efficient access to key-value pairs.
   - Thread-safety is achieved using `std::shared_mutex` to allow multiple readers or a single writer at any given time.

## Setup and Usage

### Prerequisites

- C++17 or higher

### Building the Project

1. Clone the repository:
   ```bash
   git clone git@github.com:nikolaospanagopoulos/keyValueStore.git
   cd kvstore-server
   ```
2. Compile the project:
   ```bash
   g++ -o main main.cpp KVstore.cpp
   ```
3. Run the server:
   ```bash
   ./main
   ```
4. Connect to server:
   ```
   telnet localhost 6379
   ```

### Example Commands

Once the server is running, you can interact with it using commands:

````bash
    SET mykey hello
    OK
    GET mykey
    hello
    DEL mykey
    OK
    GET mykey
    RPUSH mylist 2
    OK
    LINDEX mylist 0
    2
    RPUSH mylist 3
    OK
    LRANGE mylist 0 1
    2 3
    LPOP mylist
    OK
    LINDEX mylist 0
    3
    (nil)
      ```

````
