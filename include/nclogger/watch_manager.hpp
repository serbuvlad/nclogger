#ifndef NCLOGGER_WATCH_MANAGER_HPP
#define NCLOGGER_WATCH_MANAGER_HPP

#include <nclogger/watch.hpp>

namespace nclogger {

class WatchManager {
private:
    std::vector<Watch> watches;
    std::vector<int>   active_watches;
    std::vector<int>   active_watches_y_offset;

    int y_offset     = 0;
    int max_y_offset = 0;

    int active_watches_width  = 0;
    int active_watches_height = 0;

    WINDOW* watch_window = nullptr;

    const std::string& title();

protected:
    int  register_watch(std::string name);
    int  watch_register_value(int index, std::string name, int width);
    bool watch_set_value(int watch_index, int value_index, std::string value);

    void set_active_watch(int index);
    void add_active_watch(int index, bool acq_window = true);
    void rm_active_watch(int index);
    void add_all_active_watches();
    void clear_active_watches(bool acq_window = true);

    void increment_y_offset();
    void decrement_y_offset();
    void set_min_y_offset();
    void set_max_y_offset();

    void acquire_watch_window();
    void draw_watches();
};

}  // namespace nclogger

#endif  //NCLOGGER_WATCH_MANAGER_HPP
