#ifndef NCLOGGER_HELPMANAGER_HPP
#define NCLOGGER_HELPMANAGER_HPP

#include <map>
#include <nclogger/version.hpp>
#include <string>
#include <vector>

namespace nclogger {

class HelpManager {
private:
    struct HelpLineFragment {
        attr_t      attr;
        std::string data;
    };

    typedef std::vector<HelpLineFragment> HelpLine;

    static HelpLine simple_help_line(std::string text);

    std::vector<HelpLine> help_text;

    std::map<char, std::string> screens;
    std::map<char, std::string> watches;

    bool help_text_up_to_date = false;

    int  starty     = 0;
    int  min_starty = 0;
    void generate_help_text();

protected:
    void draw_help();

    void add_screen_to_help(char key, std::string name);
    void add_watch_to_help(char key, std::string name);

    void scroll_down_in_help();
    void scroll_up_in_help();
};

}  // namespace nclogger

#endif  //NCLOGGER_HELPMANAGER_HPP
