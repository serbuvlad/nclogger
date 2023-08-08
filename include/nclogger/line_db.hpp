#ifndef NCLOGGER_LINE_DB_HPP
#define NCLOGGER_LINE_DB_HPP

#include <nclogger/version.hpp>
#include <nclogger/line.hpp>

#include <array>
#include <deque>
#include <functional>
#include <limits>
#include <unordered_map>
#include <unordered_set>

namespace nclogger {

typedef unsigned long index_t;

class LineDB {
    typedef std::deque<std::unique_ptr<Line>> ScreenLogBucket;
    typedef std::array<ScreenLogBucket, log_level_n> ScreenBucket;


    std::unordered_map<index_t, ScreenBucket> lines;
    std::unordered_map<index_t, std::string> screen_names;

public:
    inline void create_screen(index_t id, std::string name)
    {
        lines.insert({id, ScreenBucket()});
        screen_names.insert({id, name});
    }

    inline void create_line(index_t screen_index, std::string message, LogLevel log_level)
    {
        std::size_t log_index = static_cast<std::size_t>(log_level);

        auto line = std::unique_ptr<Line>(new Line(message, screen_names.at(screen_index), log_level));

        lines.at(screen_index)[log_index].push_back(std::move(line));
    }

    typedef std::function<bool(std::unique_ptr<Line>& line)> run_on_lines_callback;

    void run_on_lines(const std::unordered_set<index_t>& screens,
                      LogLevel min_log_level,
                      run_on_lines_callback callback);
};


}  // namespace nclogger

#endif //NCLOGGER_LINE_DB_HPP
