#include <mutex>
#include <thread>
#include <atomic>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <stop_token>
#include <condition_variable>
#include <string>

struct ProgressStage {
  std::atomic_int32_t counter = 0;
  ProgressStage() = default;
  void arrive() {
    counter.fetch_add(1, std::memory_order_relaxed);
  }
};

struct ProgressBlock {
  ProgressBlock() = default;

  ProgressStage& create_stage(std::int32_t counter, const std::string& name = "") {
    if (current_stage_max.load() != 0) {
      while (current_stage.counter.load() < current_stage_max.load()) {
        std::this_thread::yield();
      }

      std::unique_lock lock(completion_mutex);
      completion_cv.wait(lock, [&] {
        return stage_fully_printed;
      });
    }

    {
      std::lock_guard lock(cout_mutex);
      if (!name.empty()) {
        std::cout << name << ":\n";
      }
    }

    stage_fully_printed = false;
    current_stage.counter.store(0, std::memory_order_release);
    current_stage_max.store(counter, std::memory_order_release);
    return current_stage;
  }

  inline static std::mutex cout_mutex;
  std::atomic_int32_t current_stage_max = 0;
  ProgressStage current_stage;

  std::mutex completion_mutex;
  std::condition_variable completion_cv;
  bool stage_fully_printed = false;

  std::jthread printer = std::jthread([this](const std::stop_token& token) {
    constexpr int bar_width = 50;
    int last_value = -1;
    int last_max = 0;

    while (!token.stop_requested()) {
      int current = current_stage.counter.load(std::memory_order_acquire);
      int max = current_stage_max.load(std::memory_order_acquire);

      if (max != last_max) {
        last_value = -1;
        last_max = max;
      }

      if (max > 0) {
        float progress = static_cast<float>(current) / max;
        int pos = static_cast<int>(bar_width * progress);
        bool completed = (current >= max);

        if (current > last_value || !completed || (completed && !stage_fully_printed)) {
          std::lock_guard lock(cout_mutex);

          std::cout << "[";
          for (int i = 0; i < bar_width; ++i) {
            if (i < pos) std::cout << "=";
            else if (i == pos) std::cout << ">";
            else std::cout << " ";
          }
          std::cout << "] ";
          std::cout << std::setw(3) << static_cast<int>(progress * 100.0) << "%";
          std::cout << " (" << current << "/" << max << ")";

          if (completed) {
            std::cout << std::endl;

            {
              std::lock_guard lk(completion_mutex);
              stage_fully_printed = true;
            }
            completion_cv.notify_one();
          } else {
            std::cout << "\r";
            std::cout.flush();
          }

          last_value = current;
        }
      }

      std::unique_lock lock(completion_mutex);
      if (completion_cv.wait_for(lock, std::chrono::seconds(1),
                                 [&] { return token.stop_requested(); })) {
        break;
      }
    }
  });
};
