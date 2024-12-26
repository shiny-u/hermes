#ifndef HERMES_CHRONO_H_
#define HERMES_CHRONO_H_

#include <chrono>
#include <string>
#include <sstream>
#include <iomanip>

namespace hermes {
  // Convert chrono time point to ISO8601
  // As reference, see
  // https://stackoverflow.com/questions/36927626/outputting-date-in-iso-8601-format
  // https://stackoverflow.com/questions/24686846/get-current-time-in-milliseconds-or-hhmmssmmm-format/35157784#35157784
  template <typename Clock>
  std::string ISO8601(const std::chrono::time_point<Clock>& time_point) {
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_point.time_since_epoch()) % 1000;
    auto t = std::chrono::system_clock::to_time_t(time_point);
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&t), "%FT%T");
    oss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    oss << std::put_time(std::localtime(&t), "%z");
    return oss.str();
  }
}  // namespace hermes

#endif  // HERMES_CHRONO_H_
