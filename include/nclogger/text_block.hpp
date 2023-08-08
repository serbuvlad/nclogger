#ifndef NCLOGGER_TEXT_BLOCK_HPP
#define NCLOGGER_TEXT_BLOCK_HPP

#include <ncursesw/ncurses.h>

#include <nclogger/version.hpp>

#include <string>
#include <vector>

#include <cassert>

namespace nclogger {

struct TextCharacter {
    enum class Type : std::uint8_t {
        NO_DRAW,
        NORMAL_WIDTH_1,
        NORMAL_WIDTH_2,
        NEWLINE
    };

    cchar_t screen_cell;
    Type    type;

    inline void draw(WINDOW* w, int y, int x)
    {
        switch (type) {
        case Type::NO_DRAW:
        case Type::NEWLINE:
        default:
            assert(false);
            break;
        
        case Type::NORMAL_WIDTH_1:
            mvwadd_wch(w, y, x, &this->screen_cell);

        case Type::NORMAL_WIDTH_2:
            mvwadd_wch(w, y, x, &this->screen_cell);

            // TODO: move cursor one more here
        }
    }
};

class TextBlock {
private:
    std::vector<TextCharacter> characters;
    std::vector<std::size_t>   line_indexes;

    int line_indexes_maxx = 0;
    int line_inedexes_maxy = 0;

    bool have_new_characters_since_last_index_update = false;

    void update_lines_index();

public:
    std::size_t get_height() const;
    std::size_t max_y_offset(const WINDOW* w) const;

    void add_string(const std::string& string,
                    attr_t             attr       = A_NORMAL,
                    short              color_pair = 0);

    void draw(WINDOW* w, std::size_t y, bool above_screen);
};

}  // namespace nclogger

#endif  //NCLOGGER_TEXT_BLOCK_HPP
