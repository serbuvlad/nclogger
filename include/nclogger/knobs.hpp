#ifndef NCLOGGER_KNOBS_HPP
#define NCLOGGER_KNOBS_HPP

#include <cstddef>
#include <nclogger/version.hpp>

namespace nclogger {

// Functionality knobs
constexpr std::size_t screen_max_history = 256;

constexpr std::size_t screen_name_width = 10;
#define screen_name_width_s "10"

// Performance knobs
constexpr std::size_t perf_screen_max_over_capacity = screen_max_history;

}  // namespace nclogger

#endif  //NCLOGGER_KNOBS_HPP
