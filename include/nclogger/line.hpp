#ifndef NCLOGGER_LINE_HPP
#define NCLOGGER_LINE_HPP

#include <fstream>
#include <memory>
#include <nclogger/version.hpp>
#include <nclogger/text_block.hpp>
#include <string>
#include <vector>

// screen names are on a separete window, to the left of the lines window

namespace nclogger {

enum class LogLevel { INFO = 0, WARN = 1, ERROR = 2, FATAL = 3 };

constexpr std::size_t log_level_n = 4;

class Line {
    std::unique_ptr<TextBlock> text_block;

    std::string message;
    std::string screen_name;

    std::string  time_string;
    std::int64_t time_ns;

    LogLevel log_level;

    inline std::string log_level_string() const;

public:
    Line(std::string message, std::string screen_name, LogLevel log_level);

    void write(std::ostream& out) const;

    inline auto get_time_ns() const -> std::int64_t {
        return time_ns;
    }

    inline auto get_log_level() const -> LogLevel {
        return log_level;
    }

    // encapsulation of text block
    inline auto get_height() const -> std::size_t {
        return text_block->get_height();
    }

    inline void draw(WINDOW* w, std::size_t y, bool above_screen) {
        return text_block->draw(w, y, above_screen);
    }
};

typedef std::vector<std::shared_ptr<Line>> Lines;

}  // namespace nclogger

#endif  //NCLOGGER_LINE_HPP
