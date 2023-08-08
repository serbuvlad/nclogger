
#include <nclogger/text_block.hpp>
#include <nclogger/line.hpp>

int main() {

    setlocale(LC_ALL, "");

    initscr();
    start_color();

    raw();
    noecho();
    keypad(stdscr, true);

    init_pair((short) nclogger::LogLevel::WARN, COLOR_YELLOW, COLOR_BLACK);
    init_pair((short) nclogger::LogLevel::ERROR, COLOR_RED, COLOR_BLACK);
    init_pair((short) nclogger::LogLevel::FATAL, COLOR_RED, COLOR_BLACK);

    nclogger::TextBlock tb;

    std::string l = "123456789123456789123456789123456789123456789";
    
    nclogger::Line line("lmao", "ell", nclogger::LogLevel::INFO);

    line.draw(stdscr, 0, false);
    
    refresh();

    getch();

    endwin();


    //aquire_status_w();

    //init_pair((short) LogLevel::WARN, COLOR_YELLOW, COLOR_BLACK);
    //init_pair((short) LogLevel::ERROR, COLOR_RED, COLOR_BLACK);
    //init_pair((short) LogLevel::FATAL, COLOR_RED, COLOR_BLACK);
}