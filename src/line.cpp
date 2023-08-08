#include <cassert>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <nclogger/knobs.hpp>
#include <nclogger/line.hpp>

namespace nclogger {

Line::Line(std::string message_, std::string screen_name_, LogLevel log_level)
    : message(std::move(message_)), screen_name(std::move(screen_name_)), log_level(log_level)
{
    {
        auto        now   = std::chrono::high_resolution_clock::now();
        std::time_t ctime = std::chrono::high_resolution_clock::to_time_t(now);
        std::tm*    parsed_time = std::localtime(&ctime);

        char printed_time[256];

        std::strftime(printed_time, 256, "[%T]", parsed_time);

        time_string = printed_time;

        time_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
                             now.time_since_epoch())
                             .count();
    }

    text_block = std::unique_ptr<TextBlock>(new TextBlock);
    text_block->add_string(time_string);
    text_block->add_string(" ");
    text_block->add_string(message);

}

inline std::string Line::log_level_string() const
{
    switch (log_level) {
    case LogLevel::INFO: return " {INFO}";
    case LogLevel::WARN: return " {WARN}";
    case LogLevel::ERROR: return "{ERROR}";
    case LogLevel::FATAL: return "{FATAL}";
    }

    assert(false);
}

void Line::write(std::ostream& out) const
{
    out << std::setw(screen_name_width) << "(" << screen_name << ") "
        << time_string << " " << log_level_string() << " " << message
        << std::endl;
}

}  // namespace nclogger
