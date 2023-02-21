#include <ncurses.h>

#include <algorithm>
#include <iostream>
#include <nclogger/logger.hpp>
#include <queue>
#include <utility>

namespace nclogger {

Logger::Logger() : ScreenManager("log.txt")
{
    initscr();
    start_color();

    raw();
    noecho();
    keypad(stdscr, true);

    aquire_status_w();

    init_pair((short) LogLevel::WARN, COLOR_YELLOW, COLOR_BLACK);
    init_pair((short) LogLevel::ERROR, COLOR_RED, COLOR_BLACK);
    init_pair((short) LogLevel::FATAL, COLOR_RED, COLOR_BLACK);

    create_composite_screen('@', "All screens");

    thread = new std::thread([this]() { run(); });
}

void Logger::run()
{
    int key;

    enum class state {
        MAIN_MENU,

        SCREEN_MENU,

        WATCH_MENU,
        WATCH_ADD_MENU,
        WATCH_ADD_CLEAR_MENU
    } state = state::MAIN_MENU;

    while ((key = getch()) != 27) {
        std::lock_guard<std::mutex> l(mux);

        bool redraw_request = false;

        if (key == KEY_RESIZE) {
            aquire_status_w();
            acquire_watch_window();

            redraw_request = true;
        } else {
            switch (state) {
            case state::MAIN_MENU:
                switch (key) {
                case 's':
                    statusmsg = "s";

                    state = state::SCREEN_MENU;
                    break;
                case 'w':
                    statusmsg = "w";

                    state = state::WATCH_MENU;
                    break;
                case 'W':
                    statusmsg = "W";

                    state = state::WATCH_ADD_MENU;
                    break;
                case KEY_UP:
                    increment_delta_history();

                    statusmsg      = "";
                    redraw_request = true;

                    break;
                case KEY_DOWN:
                    decrement_delta_history();

                    statusmsg      = "";
                    redraw_request = true;

                    break;
                case KEY_HOME:
                    set_max_delta_history();

                    statusmsg      = "";
                    redraw_request = true;

                    break;
                case KEY_END:
                    set_min_delta_history();

                    statusmsg      = "";
                    redraw_request = true;

                    break;
                case 'j':
                    decrement_y_offset();

                    statusmsg      = "";
                    redraw_request = true;

                    break;
                case 'k':
                    increment_y_offset();

                    statusmsg      = "";
                    redraw_request = true;

                    break;
                case 'J':
                    set_min_y_offset();

                    statusmsg      = "";
                    redraw_request = true;

                    break;
                case 'K':
                    set_max_y_offset();

                    statusmsg      = "";
                    redraw_request = true;

                    break;
                default: statusmsg = ""; break;
                }

                break;
            case state::SCREEN_MENU:
                if (key_to_screen.find(key) == key_to_screen.end()) {
                    statusmsg = "Could not find requested screen";

                    state = state::MAIN_MENU;
                    break;
                }

                set_active_screen(key_to_screen[key]);

                statusmsg      = "";
                redraw_request = true;
                state          = state::MAIN_MENU;

                break;
            case state::WATCH_MENU:
            case state::WATCH_ADD_MENU:
                if (key != '@' && key != '!' &&
                    key_to_screen.find(key) == key_to_screen.end()) {
                    statusmsg = "Could not find requested watch";

                    state = state::MAIN_MENU;
                    break;
                }

                if (key == '@') {
                    add_all_active_watches();
                } else if (key == '!') {
                    if (state == state::WATCH_ADD_MENU) {
                        statusmsg = "W!";
                        state     = state::WATCH_ADD_CLEAR_MENU;
                        break;
                    } else {
                        clear_active_watches();
                    }
                } else if (state == state::WATCH_ADD_MENU)
                    add_active_watch(key_to_watch[key]);
                else
                    set_active_watch(key_to_watch[key]);

                statusmsg      = "";
                state          = state::MAIN_MENU;
                redraw_request = true;

                break;
            case state::WATCH_ADD_CLEAR_MENU:
                if (key_to_screen.find(key) == key_to_screen.end()) {
                    statusmsg = "Could not find requested watch";

                    state = state::MAIN_MENU;
                    break;
                }

                rm_active_watch(key_to_screen[key]);

                statusmsg      = "";
                state          = state::MAIN_MENU;
                redraw_request = true;

                break;
            }
        }

        if (redraw_request) {
            draw_screens();

            draw_watches();
        }

        draw_status();
    }

    exit(0);
}

void Logger::draw_status()
{
    wclear(status_w);
    wmove(status_w, 0, 0);
    wprintw(status_w, "%s", statusmsg.c_str());
    wclrtoeol(status_w);
    wrefresh(status_w);
}

int Logger::request_screen(int key, std::string name)
{
    std::lock_guard<std::mutex> l(mux);

    int index = ScreenManager::request_screen(std::move(name));

    key_to_screen.insert({ key, index });

    return index;
}

int Logger::create_composite_screen(int key, std::string name)
{
    int index = ScreenManager::create_composite_screen(std::move(name));

    key_to_screen.insert({ key, index });

    return index;
}

Logger::~Logger()
{
    endwin();
}

void Logger::log(int screen_index, LogLevel log_level, std::string line)
{
    std::lock_guard<std::mutex> l(mux);

    int update = ScreenManager::screen_add_line(screen_index,
                                                log_level,
                                                std::move(line));

    if (update) {
        draw_screens();
        draw_watches();
        draw_status();
    }
}

int Logger::request_watch(int key, std::string name)
{
    std::lock_guard<std::mutex> l(mux);

    int index = WatchManager::register_watch(std::move(name));

    key_to_watch.insert({ key, index });

    return index;
}

int Logger::register_watch_value(int         watch_index,
                                 std::string value_name,
                                 int         width)
{
    std::lock_guard<std::mutex> l(mux);

    return WatchManager::watch_register_value(watch_index,
                                              std::move(value_name),
                                              width);
}

void Logger::set_watch_value(int         watch_index,
                             int         value_index,
                             std::string value)
{
    std::lock_guard<std::mutex> l(mux);

    bool update = WatchManager::watch_set_value(watch_index,
                                                value_index,
                                                std::move(value));

    if (update) {
        draw_watches();
        draw_status();
    }
}

void Logger::aquire_status_w()
{
    int y, x;
    getmaxyx(stdscr, y, x);

    if (status_w != nullptr) {
        wclear(status_w);
        wrefresh(status_w);
        delwin(status_w);
    }

    status_w = newwin(1, x, y - 1, 0);
}

}  // namespace nclogger
