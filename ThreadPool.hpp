#pragma once
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {
public:
  ThreadPool(size_t numThreads) {
    for (size_t i = 0; i < numThreads; ++i) {
      workers.emplace_back([this] {
        while (true) {
          std::function<void()> task;
          {
            std::unique_lock<std::mutex> lock(queueMutex);
            // thread waits until there are tasks in the  queue or the pool
            // needs to stop
            condition.wait(lock, [this] { return !tasks.empty() || stop; });
            if (stop && tasks.empty()) {
              return;
            }
            task = std::move(tasks.front());
            tasks.pop();
          }
          task();
        }
      });
    }
  }

  template <typename F> void enqueue(F &&task) {
    {
      std::unique_lock<std::mutex> lock(queueMutex);
      // preserve lvalue or rvalue (std::forward)
      tasks.emplace(std::forward<F>(task));
    }
    condition.notify_one();
  }

  ~ThreadPool() {
    {
      std::unique_lock<std::mutex> lock(queueMutex);
      stop = true;
    }
    condition.notify_all();
    for (std::thread &worker : workers)
      worker.join();
  }


  void shutdown() {
    {
      std::unique_lock<std::mutex> lock(queueMutex);
      stop = true;
    }
    condition.notify_all();

    // Join all worker threads to ensure proper cleanup
    for (std::thread &worker : workers) {
      if (worker.joinable()) {
        worker.join();
      }
    }
  }

private:
  std::vector<std::thread> workers;
  std::condition_variable condition;
  std::queue<std::function<void()>> tasks;
  std::mutex queueMutex;
  bool stop = false;
};
