#include <mutex>
#include <thread>
#include <atomic>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <stop_token>

struct ProgressStage {
  std::atomic_int32_t counter = 0;
  ProgressStage() = default;
  void arrive() {
    counter.fetch_add(1, std::memory_order_relaxed);
  }
};

struct ProgressBlock {
  ProgressBlock() = default;

  ProgressStage& create_stage(std::int32_t counter) {
    if (auto max_old = current_stage_max.load(); max_old != 0) {
      while (current_stage.counter.load(std::memory_order_acquire) < max_old) {
        std::this_thread::yield();
      }
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
    current_stage.counter.store(0, std::memory_order_release);
    current_stage_max.store(counter, std::memory_order_release);
    return current_stage;
  }

  inline static std::mutex cout_mutex;
  std::atomic_int32_t current_stage_max = 0;
  ProgressStage current_stage;

  std::jthread printer = std::jthread([this](const std::stop_token& token) {
    constexpr int bar_width = 50;
    int last_value = -1;

    while (!token.stop_requested()) {
      int current = current_stage.counter.load(std::memory_order_acquire);
      int max = current_stage_max.load(std::memory_order_acquire);

      if (current > last_value || (current >= max && max > 0)) {
        std::lock_guard lock(cout_mutex);
        float progress = (max > 0) ? static_cast<float>(current) / max : 0.0f;
        int pos = static_cast<int>(bar_width * progress);

        std::cout << "[";
        for (int i = 0; i < bar_width; ++i) {
          if (i < pos) std::cout << "=";
          else if (i == pos) std::cout << ">";
          else std::cout << " ";
        }
        std::cout << "] ";
        std::cout << std::setw(3) << static_cast<int>(progress * 100.0) << "%";
        std::cout << " (" << current << "/" << max << ")";

        if (current >= max && max > 0) {
          std::cout << std::endl;
          last_value = -1;
        } else {
          std::cout << "\r";
          std::cout.flush();
          last_value = current;
        }
      }
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  });
};
