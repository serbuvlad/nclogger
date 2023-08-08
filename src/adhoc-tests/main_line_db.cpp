#include <nclogger/line_db.hpp>

#include <iostream>

#include <ncursesw/ncurses.h>

int main()
{
    /*setlocale(LC_ALL, "");

    initscr();
    start_color();

    raw();
    noecho();
    keypad(stdscr, true);*/

    // ----------------------------

    init_pair((short) nclogger::LogLevel::WARN, COLOR_YELLOW, COLOR_BLACK);
    init_pair((short) nclogger::LogLevel::ERROR, COLOR_RED, COLOR_BLACK);
    init_pair((short) nclogger::LogLevel::FATAL, COLOR_RED, COLOR_BLACK);

    nclogger::LineDB line_db;

    line_db.create_screen(0, "ana");
    line_db.create_screen(1, "braum");
    line_db.create_screen(2, "cristi");

    line_db.create_line(0, "lol", nclogger::LogLevel::INFO);
    line_db.create_line(0, "lmao", nclogger::LogLevel::ERROR);
    line_db.create_line(1, "lol1", nclogger::LogLevel::WARN);
    line_db.create_line(2, "lol2", nclogger::LogLevel::WARN);
    line_db.create_line(1, "lol3", nclogger::LogLevel::WARN);
    line_db.create_line(2, "lol4", nclogger::LogLevel::ERROR);

    std::unordered_set<std::size_t> screens{0, 2};

    std::ostream& out = std::cout;

    line_db.run_on_lines(screens, nclogger::LogLevel::ERROR, [&](std::unique_ptr<nclogger::Line>& line) -> bool {
        line->write(out);

        return false;
    });

    // ----------------------------

    /*refresh();

    getch();

    endwin();*/
}