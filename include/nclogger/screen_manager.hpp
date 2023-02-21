#ifndef NCLOGGER_SCREEN_MANAGER_HPP
#define NCLOGGER_SCREEN_MANAGER_HPP

#include <nclogger/screen.hpp>

namespace nclogger {

class ScreenManager {
private:
    std::vector<Screen>                  screens;
    std::vector<std::shared_ptr<Canvas>> composites;

    int active_screen = -1;
    int delta_history = 0;

    std::ofstream logfile;

protected:
    explicit ScreenManager(const std::string& file_path);
    ~ScreenManager();

    bool screen_add_line(int         screen_index,
                         LogLevel    log_level,
                         std::string line);
    ;
    int request_screen(std::string name);
    int create_composite_screen(std::string name);

    void increment_delta_history();
    void decrement_delta_history();
    void set_min_delta_history();
    void set_max_delta_history();

    void set_active_screen(int index);

    void draw_screens();
};

}  // namespace nclogger

#endif  //NCLOGGER_SCREEN_MANAGER_HPP
