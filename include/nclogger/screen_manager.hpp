#ifndef NCLOGGER_SCREEN_MANAGER_HPP
#define NCLOGGER_SCREEN_MANAGER_HPP

#include <cstdio>
#include <nclogger/screen.hpp>
#include <nclogger/version.hpp>

#include <unordered_map>
#include <unordered_set>

namespace nclogger {

class ScreenManager {
private:
    /*std::vector<std::shared_ptr<Screen>> screens;

    int         active_screen = -1;
    std::size_t delta_history = 0;

    std::ostream* logfile;
    bool          own_logfile = false;*/

    std::unordered_map<int, std::unordered_set<std::size_t>> screens;

protected:
    ScreenManager();
    explicit ScreenManager(std::ostream& logfile);
    explicit ScreenManager(const std::string& file_path);
    ~ScreenManager();

    bool screen_log(int                screen_index,
                    LogLevel           log_level,
                    const std::string& message);
    int  request_screen(std::string name);
    int  create_composite_screen(std::string name);

    void increment_delta_history(const WINDOW* screen_w);
    void decrement_delta_history();
    void set_min_delta_history();
    void set_max_delta_history(const WINDOW* screen_w);

    void set_active_screen(int index);

    void draw_screens(WINDOW* screen_w, WINDOW* title_w);

    const std::string& screen_get_name(int index) const;
};

}  // namespace nclogger

#endif  //NCLOGGER_SCREEN_MANAGER_HPP
