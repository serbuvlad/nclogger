#include <algorithm>
#include <nclogger/watch_manager.hpp>

namespace nclogger {

int WatchManager::register_watch(std::string name)
{
    watches.emplace_back(std::move(name));

    return static_cast<int>(watches.size()) - 1;
}

int WatchManager::watch_register_value(int index, std::string name, int width)
{
    return watches.at(index).register_value(std::move(name), width);
}

bool WatchManager::watch_set_value(int         watch_index,
                                   int         value_index,
                                   std::string value)
{
    watches.at(watch_index).set_value(value_index, std::move(value));

    if (std::find(active_watches.begin(), active_watches.end(), watch_index) ==
        active_watches.end())
        return false;
    return true;
}

void WatchManager::acquire_watch_window()
{
    y_offset = 0;

    if (watch_window != nullptr) {
        wclear(watch_window);
        wborder(watch_window, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
        wrefresh(watch_window);
        delwin(watch_window);

        watch_window = nullptr;
    }

    if (active_watches_width == 0 || active_watches_height == 0) {
        return;
    }

    int x, y;
    getmaxyx(stdscr, y, x);

    int window_width  = active_watches_width + 2;
    int window_height = std::min(y - 2, active_watches_height + 2);

    watch_window = newwin(window_height, window_width, 0, x - window_width - 1);

    max_y_offset = std::max(0, active_watches_height - (window_height - 2));
}

void WatchManager::draw_watches()
{
    if (watch_window == nullptr)
        return;

    wclear(watch_window);
    box(watch_window, 0, 0);

    wattron(watch_window, A_BOLD);
    mvwprintw(watch_window, 0, 1, "%s", title().c_str());
    wattroff(watch_window, A_BOLD);

    int window_width  = active_watches_width + 2;
    int window_height = active_watches_height - max_y_offset + 2;

    if (active_watches.size() == 1) {
        watches.at(active_watches[0])
            .draw(watch_window, 1, window_height - 1, y_offset, false);

        wrefresh(watch_window);
        return;
    }

    int y = 0;

    for (int active_watch : active_watches) {
        Watch& watch = watches[active_watch];

        if (y + watch.get_height() < y_offset) {
            y += watch.get_height();
            continue;
        }

        if (y - y_offset >= window_height - 2)
            break;

        int delta_rows = std::max(0, y_offset - y);

        if (delta_rows > 0)
            watch.draw(watch_window, 1, window_height - 1, delta_rows, true);
        else
            watch.draw(watch_window,
                       y - y_offset + 1,
                       window_height - 1,
                       0,
                       true);

        y += watch.get_height() + 2;
    }

    wattron(watch_window, A_BOLD);
    if (y_offset > 0)
        mvwprintw(watch_window, 0, window_width - 2, "^");

    if (y_offset < max_y_offset)
        mvwprintw(watch_window, window_height - 1, window_width - 2, "v");

    wattroff(watch_window, A_BOLD);

    wrefresh(watch_window);
}

void WatchManager::set_active_watch(int index)
{
    clear_active_watches(false);

    add_active_watch(index);
}

void WatchManager::add_active_watch(int index, bool acq_window)
{
    for (auto i : active_watches) {
        if (i == index)
            return;
    }

    Watch& watch = watches.at(index);

    active_watches.push_back(index);
    active_watches_y_offset.push_back(0);

    if (watch.get_width() > active_watches_width)
        active_watches_width = watch.get_width();

    if (active_watches.size() == 1)
        active_watches_height = watch.get_height();
    else if (active_watches.size() == 2)
        active_watches_height += watch.get_height() + 3;
    else
        active_watches_height += watch.get_height() + 2;

    if (acq_window)
        acquire_watch_window();
}

/// \internal
///
/// \brief Remove one watch from the active pool.
///
/// This can be done in a much less expensive method but eh.
///
/// \param index
void WatchManager::rm_active_watch(int index)
{
    int removed_index = -1;

    for (int i = 0; i < active_watches.size(); i++) {
        if (active_watches[i] == index)
            removed_index = i;
    }

    if (removed_index == -1)
        return;

    active_watches.erase(active_watches.begin() + removed_index);

    std::vector<int> old_active_watches(active_watches);

    clear_active_watches(false);
    for (auto i : old_active_watches)
        add_active_watch(i, false);

    acquire_watch_window();
}

void WatchManager::add_all_active_watches()
{
    for (int i = 0; i < watches.size(); i++)
        add_active_watch(i, false);

    acquire_watch_window();
}

void WatchManager::clear_active_watches(bool acq_window)
{
    active_watches.resize(0);
    active_watches_y_offset.resize(0);
    active_watches_width  = 0;
    active_watches_height = 0;

    if (acq_window)
        acquire_watch_window();
}

const std::string& WatchManager::title()
{
    if (active_watches.size() == 1)
        return watches.at(active_watches[0]).get_title();

    static const std::string w = "Watches";

    return w;
}

void WatchManager::increment_y_offset()
{
    y_offset++;

    if (y_offset > max_y_offset)
        y_offset = max_y_offset;
}

void WatchManager::decrement_y_offset()
{
    y_offset--;

    if (y_offset < 0)
        y_offset = 0;
}

void WatchManager::set_min_y_offset()
{
    y_offset = 0;
}

void WatchManager::set_max_y_offset()
{
    y_offset = max_y_offset;
}

}  // namespace nclogger
