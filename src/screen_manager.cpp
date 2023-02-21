#include <ncurses.h>

#include <nclogger/screen_manager.hpp>

namespace nclogger {

ScreenManager::ScreenManager(const std::string& file_path) : logfile(file_path)
{
}

ScreenManager::~ScreenManager()
{
    logfile.flush();
}

bool ScreenManager::screen_add_line(int         screen_index,
                                    LogLevel    log_level,
                                    std::string line)
{

    screens.at(screen_index).log(log_level, std::move(line));

    if (screen_index == active_screen ||
        (active_screen < 0 && screens[screen_index].has_composite_screen(
                                  composites[~active_screen]))) {
        if (delta_history > 0)
            delta_history++;

        return true;
    }

    return false;
}

int ScreenManager::request_screen(std::string name)
{
    screens.emplace_back(std::move(name), logfile);

    int index = static_cast<int>(screens.size() - 1);

    screens[index].add_composite_screen(composites[0]);

    return index;
}

int ScreenManager::create_composite_screen(std::string name)
{
    composites.push_back(std::make_shared<Canvas>(std::move(name)));

    return ~static_cast<int>(composites.size() - 1);
}

void ScreenManager::increment_delta_history()
{
    delta_history++;
}

void ScreenManager::decrement_delta_history()
{
    delta_history = std::max(delta_history - 1, 0);
}

void ScreenManager::set_min_delta_history()
{
    delta_history = 0;
}

void ScreenManager::set_max_delta_history()
{
    delta_history = std::numeric_limits<int>::max();
}

void ScreenManager::set_active_screen(int index)
{
    active_screen = index;
}

void ScreenManager::draw_screens()
{
    clear();

    if (active_screen < 0) {
        composites[~active_screen]->draw(delta_history);
    } else {
        screens[active_screen].draw(delta_history);
    }

    refresh();
}

}  // namespace nclogger
