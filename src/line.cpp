#include <ncurses.h>

#include <cassert>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <nclogger/knobs.hpp>
#include <nclogger/line.hpp>

namespace nclogger {

std::string Line::log_level_string() const
{
    switch (log_level) {
    case LogLevel::INFO: return " {INFO}";
    case LogLevel::WARN: return " {WARN}";
    case LogLevel::ERROR: return "{ERROR}";
    case LogLevel::FATAL: return "{FATAL}";
    }

    assert(false);
}

void Line::draw(int y, bool show_screen_name) const
{
    move(y, 0);
    if (show_screen_name)
        printw("%" screen_name_width_s "s ", ("(" + screen_name + ")").c_str());

    printw("%s ", time_string.c_str());

    if (log_level != LogLevel::INFO)
        attron(COLOR_PAIR((int) log_level));

    printw("%s", message.c_str());

    if (log_level != LogLevel::INFO)
        attroff(COLOR_PAIR((int) log_level));
}

void Line::write(std::ostream& out) const
{
    out << std::setw(screen_name_width) << "(" + screen_name + ") "
        << time_string << " " << log_level_string() << " " << message
        << std::endl;
}

std::shared_ptr<const Line> Line::create(std::string message,
                                         std::string screen_name,
                                         LogLevel    log_level)
{
    auto        now   = std::chrono::high_resolution_clock::now();
    std::time_t ctime = std::chrono::high_resolution_clock::to_time_t(now);
    std::tm*    parsed_time = std::localtime(&ctime);

    char printed_time[256];

    std::strftime(printed_time, 256, "[%T]", parsed_time);

    return std::make_shared<Line>(
        Line{ .message     = std::move(message),
              .screen_name = std::move(screen_name),

              .time_string = printed_time,
              .time_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
                             now.time_since_epoch())
                             .count(),

              .log_level = log_level });
}

}  // namespace nclogger
