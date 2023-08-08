
#include <nclogger/text_block.hpp>

int main() {

    setlocale(LC_ALL, "");

    initscr();
    start_color();

    raw();
    noecho();
    keypad(stdscr, true);

    nclogger::TextBlock tb;

    std::string l = "123456789123456789123456789123456789123456789\n";
    tb.add_string(l);
    tb.add_string(l);
    tb.draw(stdscr, 0, false);

    refresh();

    getch();

    endwin();


    //aquire_status_w();

    //init_pair((short) LogLevel::WARN, COLOR_YELLOW, COLOR_BLACK);
    //init_pair((short) LogLevel::ERROR, COLOR_RED, COLOR_BLACK);
    //init_pair((short) LogLevel::FATAL, COLOR_RED, COLOR_BLACK);
}