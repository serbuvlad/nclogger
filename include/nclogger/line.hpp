#ifndef NCLOGGER_LINE_HPP
#define NCLOGGER_LINE_HPP

#include <fstream>
#include <memory>
#include <string>
#include <vector>

namespace nclogger {

enum class LogLevel { INFO = 0, WARN = 1, ERROR = 2, FATAL = 3 };

struct Line {
    std::string message;
    std::string screen_name;

    std::string  time_string;
    std::int64_t time_ns;

    LogLevel log_level;

    [[nodiscard]] std::string log_level_string() const;
    void                      draw(int y, bool show_screen_name) const;
    void                      write(std::ostream& out) const;

    static std::shared_ptr<const Line> create(std::string message,
                                              std::string screen_name,
                                              LogLevel    log_level);
};

typedef std::vector<std::shared_ptr<const Line>> Lines;

}  // namespace nclogger

#endif  //NCLOGGER_LINE_HPP
