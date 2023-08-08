#include <unicode/brkiter.h>
#include <unicode/unistr.h>

#include <cwchar>
#include <memory>
#include <nclogger/text_block.hpp>

// width of each terminal cell in wide characters, not including L'\0'.
// ncurses defines this.
// supposedly, other curses do not, but must support at least 5.
#ifndef CCHARW_MAX
#define CCHARW_MAX 5
#else
static_assert(CCHARW_MAX >= 5, "CCHARW_MAX defined as less than 5 in curses");
#endif

namespace nclogger {

static inline uint8_t size_in_utf8_bytes(UChar32 c)
{
    if (c < 0x80)
        return 1;
    if (c < 0x800)
        return 2;
    if (c < 0x10000)
        return 3;

    return 4;
}

static_assert(sizeof(wchar_t) == 4, "wchar_t must be UTF-32");

static inline void read_graphene(wchar_t dst[CCHARW_MAX + 1],
                                 UText*  src,
                                 int     start,
                                 int     stop)
{
    int i = start;
    int j = 0;

    while (i < stop && j < CCHARW_MAX) {
        UChar32 code_point = utext_char32At(src, i);

        dst[j++] = static_cast<wchar_t>(code_point);

        i += size_in_utf8_bytes(code_point);
    }

    dst[j] = L'\0';
}

static TextCharacter::Type format_control(wchar_t dst[CCHARW_MAX + 1], UChar32 ctl)
{
    TextCharacter::Type type;

    switch (u_charType(ctl)) {
    case U_CONTROL_CHAR:

        // ASCII control codes
        if (ctl < 0x20) {
            dst[0] = L'\x2400' + ctl; // Unicode control pictures
            
            type = TextCharacter::Type::NORMAL_WIDTH_1;

        // ASCII DEL
        } else if (ctl == 0x7F) {
            dst[0] = L'\x2421'; // Unicode control picture

            type = TextCharacter::Type::NORMAL_WIDTH_1;

        // C1 control codes
        } else {
            dst[0] = L'\xFFFD'; // Unicode replacement char

            type = TextCharacter::Type::NORMAL_WIDTH_2;
        }

        break;
    case U_FORMAT_CHAR:
        dst[0] = L'\xFFFD'; // Unicode replacement char


        type = TextCharacter::Type::NORMAL_WIDTH_2;

        break;
    default:
        assert(false);
    }

    dst[1] = L'\0';

    return type;
}

static inline uint8_t unicode_width(UChar32 c)
{
    switch (u_getIntPropertyValue(c, UCHAR_EAST_ASIAN_WIDTH)) {
    case U_EA_AMBIGUOUS:
    case U_EA_FULLWIDTH:
    case U_EA_WIDE: return 2;
    default: return 1;
    }
}

static TextCharacter create_text_character(UText* src,
                                           int    start,
                                           int    stop,
                                           attr_t attr,
                                           short  color_pair)
{
    wchar_t c[CCHARW_MAX + 1];

    read_graphene(c, src, start, stop);

    TextCharacter tc{};

    UChar32 main_code_point = c[0];

    if (main_code_point == L'\n') {
        tc.type = TextCharacter::Type::NEWLINE;
    } else if (u_charType(main_code_point) == U_CONTROL_CHAR ||
               u_charType(main_code_point) == U_FORMAT_CHAR) {

        tc.type = format_control(c, main_code_point);
    } else if (unicode_width(main_code_point) == 1) {
        tc.type = TextCharacter::Type::NORMAL_WIDTH_1;
    } else {
        tc.type = TextCharacter::Type::NORMAL_WIDTH_2;
    }

    setcchar(&tc.screen_cell, c, attr, color_pair, nullptr);

    return tc;
}

void TextBlock::add_string(const std::string& string,
                           attr_t             attr,
                           short              color_pair)
{
    UErrorCode u_error_code = U_ZERO_ERROR;
    UText*     text;

    text = utext_openUTF8(nullptr,
                          string.c_str(),
                          static_cast<std::int64_t>(string.size()),
                          &u_error_code);



    // why is character breaking dependent on locale???
    std::unique_ptr<icu::BreakIterator> iter =
        std::unique_ptr<icu::BreakIterator>(
            icu::BreakIterator::createCharacterInstance(icu::Locale::getUS(),
                                                        u_error_code));

    iter->setText(text, u_error_code);

    int last = iter->last();
    int next;

    for (int i = iter->first(); i != last; i = next) {
        next = iter->next();

        characters.push_back(
            create_text_character(text, i, next, attr, color_pair));
    }

    utext_close(text);

    have_new_characters_since_last_index_update = true;
}

void TextBlock::update_lines_index()
{
    line_indexes.resize(0);

    line_indexes.push_back(0);

    int maxy, maxx;
    getmaxyx(stdscr, maxy, maxx);

    line_inedexes_maxy = maxy;
    line_indexes_maxx = maxx;

    int x = 0;

    for (std::size_t i = 0; i < characters.size(); ++i) {
        auto& character = characters[i];

        switch (character.type) {
        case TextCharacter::Type::NO_DRAW: break;
        case TextCharacter::Type::NORMAL_WIDTH_2: ++x;
        case TextCharacter::Type::NORMAL_WIDTH_1:
            ++x;

            if (x == maxx) {
                x = 0;
                line_indexes.push_back(i + 1);
            }

            break;
        case TextCharacter::Type::NEWLINE:
            x = 0;
            line_indexes.push_back(i + 1);

            break;
        }
    }
}

std::size_t TextBlock::get_height() const
{
    return line_indexes.size();
}

// ????
std::size_t TextBlock::max_y_offset(const WINDOW* w) const
{
    std::size_t maxy = getmaxy(w);

    if (get_height() < maxy)
        return 0;
    
    return maxy - get_height();
}

void TextBlock::draw(WINDOW* w, std::size_t y, bool above_screen)
{
    int x = 0;
    int maxy, maxx;

    getmaxyx(w, maxy, maxx);

    if (have_new_characters_since_last_index_update || maxy != line_inedexes_maxy || maxx != line_indexes_maxx)
        update_lines_index();


    std::size_t first_character_index = 0;

    if (above_screen) {
        if (y >= line_indexes.size())
            return;
    
        first_character_index = line_indexes.at(y);
        y = 0;
    }

    for (std::size_t i = first_character_index; i < characters.size(); ++i) {
        TextCharacter& character = characters[i];

        if (character.type == TextCharacter::Type::NORMAL_WIDTH_1 ||
            character.type == TextCharacter::Type::NORMAL_WIDTH_2) {

            characters[i].draw(w, y, x);

            if (character.type == TextCharacter::Type::NORMAL_WIDTH_2)
                x += 2;
            else
                ++x;

            if (x >= maxx) {
                x = 0;
                ++y;
            }
        }

        if (character.type == TextCharacter::Type::NEWLINE) {
            ++y;
            x = 0;
        }

        if (y >= maxy)
            break;
    }
}

}  // namespace nclogger
