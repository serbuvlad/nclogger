#include <cassert>
#include <nclogger/screen_manager.hpp>

namespace nclogger {

ScreenManager::ScreenManager() : logfile(nullptr)
{
    create_composite_screen("All logs");
}

ScreenManager::ScreenManager(std::ostream& logfile) : logfile(&logfile)
{
    create_composite_screen("All logs");
}

ScreenManager::ScreenManager(const std::string& file_path)
    : logfile(new std::ofstream(file_path)), own_logfile(true)
{
    create_composite_screen("All logs");
}

ScreenManager::~ScreenManager()
{
    if (logfile != nullptr) {
        logfile->flush();

        if (own_logfile)
            delete logfile;
    }
}

bool ScreenManager::screen_log(int                screen_index,
                               LogLevel           log_level,
                               const std::string& line)
{
    screens.at(screen_index)->log(log_level, line);
}

int ScreenManager::request_screen(std::string name)
{
    screens.push_back(std::make_shared<Screen>(std::move(name), logfile));

    int index = static_cast<int>(screens.size() - 1);

    screens[index]->add_composite_screen(screens[0]);

    return index;
}

int ScreenManager::create_composite_screen(std::string name)
{
    screens.push_back(std::make_shared<Screen>(std::move(name), nullptr, true));

    return static_cast<int>(screens.size() - 1);
}

void ScreenManager::increment_delta_history(const WINDOW* screen_w)
{
    ++delta_history;

    if (delta_history > screens[active_screen]->max_y_offset(screen_w))
        delta_history = screens[active_screen]->max_y_offset(screen_w);
}

void ScreenManager::decrement_delta_history()
{
    if (delta_history == 0)
        return;

    --delta_history;
}

void ScreenManager::set_max_delta_history(const WINDOW* screen_w)
{
    delta_history = screens[active_screen]->max_y_offset(screen_w);
}

void ScreenManager::set_min_delta_history()
{
    delta_history = 0;
}

void ScreenManager::set_active_screen(int index)
{
    active_screen = index;

    assert(index >= 0 && index < screens.size());
}

void ScreenManager::draw_screens(WINDOW* screen_w, WINDOW* title_w)
{
    wclear(screen_w);

    screens[active_screen]->draw(
        screen_w,
        screens[active_screen]->max_y_offset(screen_w) - delta_history);

    wrefresh(screen_w);

    wclear(title_w);
    wattron(title_w, A_BOLD);
    mvwprintw(title_w, 0, 0, "%s", "title here");
    wattroff(title_w, A_BOLD);
    wrefresh(title_w);
}

const std::string& ScreenManager::screen_get_name(int index) const
{
    return screens.at(index)->get_name();
}

}  // namespace nclogger
