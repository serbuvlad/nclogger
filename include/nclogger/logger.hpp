///
/// \file nlogger.hpp
/// \brief nclogger::Logger class.

#ifndef NCLOGGER_LOGGER_HPP
#define NCLOGGER_LOGGER_HPP

#include <mutex>
#include <nclogger/canvas.hpp>
#include <nclogger/help_manager.hpp>
#include <nclogger/screen.hpp>
#include <nclogger/screen_manager.hpp>
#include <nclogger/version.hpp>
#include <nclogger/watch_manager.hpp>
#include <thread>
#include <unordered_map>
#include <vector>

///
/// \brief ncurses logger
namespace nclogger {

///
/// \brief Logger class
///
/// Does logging
class Logger final : public ScreenManager, WatchManager, HelpManager {
private:
    [[maybe_unused]] std::thread* thread;

    std::unordered_map<int, int> key_to_screen;
    std::unordered_map<int, int> key_to_watch;

    std::mutex mux;

    std::string statusmsg;
    WINDOW*     status_w = nullptr;

    bool drawing_main_screen = true;

    void run();

    void draw_status();
    void aquire_status_w();

    int create_composite_screen(int key, std::string name);

public:
    Logger();
    ~Logger();

    void log(int screen_index, LogLevel log_level, std::string line);
    int  request_screen(int key, std::string name);

    int  request_watch(int key, std::string name);
    int  register_watch_value(int         watch_index,
                              std::string value_name,
                              int         width);
    void set_watch_value(int watch_index, int value_index, std::string value);
};

}  // namespace nclogger

#endif  //NCLOGGER_LOGGER_HPP
