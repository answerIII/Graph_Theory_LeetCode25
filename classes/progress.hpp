#include <mutex>
#include <thread>
#include <atomic>
#include <cstdint>
#include <iomanip>
#include <iostream>

struct ProgressStage {
  std::atomic_int32_t counter = 0;
  ProgressStage() = default;
  void arrive() { ++counter; }
};

struct ProgressBlock {
  ProgressBlock() = default;

  ProgressStage & create_stage(std::int32_t counter) {
    if (current_stage_max != 0) {
      while (current_stage_max != current_stage.counter.load());
    }
    current_stage.counter = 0;
    current_stage_max = counter;
    return current_stage;
  }

  inline static std::mutex cout_mutex;
  std::int32_t current_stage_max = 0;
  ProgressStage current_stage {};

  std::jthread printer = std::jthread([this](const std::stop_token &token) {
    constexpr int bar_width = 50;

    std::lock_guard lock(cout_mutex);

    int last_value = -1;

    while (!token.stop_requested()) {
      int current = current_stage.counter.load();
      int max = current_stage_max;

      float progress = static_cast<float>(current) / max;
      int pos = static_cast<int>(bar_width * progress);

      if (current > last_value || (current >= max && max > 0)) {
        std::cout << "[";
        for (int i = 0; i < bar_width; ++i) {
          if (i < pos) std::cout << "=";
          else if (i == pos) std::cout << ">";
          else std::cout << " ";
        }
        std::cout << "] ";
        std::cout << std::setw(3) << static_cast<int>(progress * 100.0) << "%";
        std::cout << " (" << current << "/" << max << ")";

        if (current >= max) {
          std::cout << std::endl;
          last_value = -1;
        } else {
          std::cout << "\r";
          std::cout.flush();
        }
      }

      last_value = current;
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  });
};

