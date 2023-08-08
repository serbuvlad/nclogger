#include <nclogger/help_manager.hpp>
#include <nclogger/to_string.hpp>

namespace nclogger {

void HelpManager::generate_help_text()
{
    help_text.resize(0);

    help_text.push_back(simple_help_line("COMMANDS"));
    help_text.push_back(simple_help_line(""));
    help_text.push_back(simple_help_line("SCREEN COMMANDS"));
    help_text.push_back(simple_help_line(""));
    help_text.push_back(simple_help_line("s[screen] : show screen"));
    help_text.push_back(
        simple_help_line("KEY_UP    : scroll up in screen history"));
    help_text.push_back(
        simple_help_line("KEY_DOWN  : scroll down in screen history"));
    help_text.push_back(
        simple_help_line("KEY_HOME  : scroll to top in screen history"));
    help_text.push_back(
        simple_help_line("KEY_END   : scroll to bottom in screen history"));
    help_text.push_back(simple_help_line(""));
    help_text.push_back(simple_help_line("SCREENS"));
    help_text.push_back(simple_help_line(""));

    for (auto& pair : screens) {
        char               key  = pair.first;
        const std::string& name = pair.second;

        help_text.push_back(
            simple_help_line(nclogger::to_string(key) + " : " + name));
    }

    help_text.push_back(simple_help_line(""));
    help_text.push_back(simple_help_line("WATCH COMMANDS"));
    help_text.push_back(simple_help_line(""));
    help_text.push_back(simple_help_line("w[watch]  : show one watch"));
    help_text.push_back(simple_help_line("w@        : show all watches"));
    help_text.push_back(simple_help_line("w!        : hide all watches"));
    help_text.push_back(
        simple_help_line("W[watch]  : add watch to shown watches"));
    help_text.push_back(simple_help_line("W![watch] : hide watch"));
    help_text.push_back(
        simple_help_line("k         : scroll up in watch view"));
    help_text.push_back(
        simple_help_line("j         : scroll down in watch view"));
    help_text.push_back(
        simple_help_line("K         : scroll to top in watch view"));
    help_text.push_back(
        simple_help_line("J         : scroll to bottom in watch view"));
    help_text.push_back(simple_help_line(""));
    help_text.push_back(simple_help_line("WATCHES"));
    help_text.push_back(simple_help_line(""));

    for (auto& pair : watches) {
        char               key  = pair.first;
        const std::string& name = pair.second;

        help_text.push_back(
            simple_help_line(nclogger::to_string(key) + " : " + name));
    }

    help_text_up_to_date = true;

    help_text.push_back(simple_help_line(""));
    help_text.push_back(simple_help_line("OTHER COMMANDS"));
    help_text.push_back(simple_help_line(""));
    help_text.push_back(simple_help_line("h : show/hide this screen"));
}

HelpManager::HelpLine HelpManager::simple_help_line(std::string text)
{
    return HelpLine{ HelpLineFragment{ A_NORMAL, std::move(text) } };
}

void HelpManager::draw_help()
{
    if (!help_text_up_to_date)
        generate_help_text();

    int maxy;

    maxy = getmaxy(stdscr) - 2;

    if (starty > maxy - static_cast<int>(help_text.size()))
        starty = std::min(maxy - static_cast<int>(help_text.size()), 0);

    int y = starty;

    clear();

    for (auto& line : help_text) {
        if (y < 0) {
            y++;
            continue;
        }

        if (y == maxy) {
            break;
        }

        int x = 0;

        for (auto& fragment : line) {
            attron(fragment.attr);
            mvprintw(y, x, "%s", fragment.data.c_str());
            attroff(fragment.attr);

            x += static_cast<int>(fragment.data.size());
        }

        y++;
    }

    refresh();
}

void HelpManager::add_screen_to_help(char key, std::string name)
{
    screens.insert({ key, std::move(name) });

    help_text_up_to_date = false;
}

void HelpManager::add_watch_to_help(char key, std::string name)
{
    watches.insert({ key, std::move(name) });

    help_text_up_to_date = false;
}

void HelpManager::scroll_up_in_help()
{
    starty--;
}

void HelpManager::scroll_down_in_help()
{
    if (starty < 0)
        starty++;
}

}  // namespace nclogger
