#include <chrono>
#include <iostream>

class PerformanceTimer {
 public:
  PerformanceTimer()
      : start_timepoint(std::chrono::high_resolution_clock::now()) {}

  void reset() { start_timepoint = std::chrono::high_resolution_clock::now(); }

  double elapsedMilliseconds() const {
    return std::chrono::duration<double, std::milli>(
               std::chrono::high_resolution_clock::now() - start_timepoint)
        .count();
  }

  double elapsedSeconds() const {
    return std::chrono::duration<double>(
               std::chrono::high_resolution_clock::now() - start_timepoint)
        .count();
  }

  ~PerformanceTimer() {
    auto endTimepoint = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        endTimepoint - start_timepoint);
    std::cout << "Time taken: " << duration.count() << " ms." << std::endl;
  }

 private:
  std::chrono::time_point<std::chrono::high_resolution_clock> start_timepoint;
};