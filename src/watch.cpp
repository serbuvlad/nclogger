#include <nclogger/watch.hpp>

namespace nclogger {

Watch::Watch(std::string name) : name(std::move(name)) {}

int Watch::register_value(std::string value_name, int value_width)
{
    int total_width = value_width + static_cast<int>(value_name.size()) + 2;

    values.push_back(Value{ std::move(value_name), "", value_width });

    if (total_width > width)
        width = total_width;

    return static_cast<int>(values.size() - 1);
}

void Watch::set_value(int index, std::string value)
{
    values.at(index).value = std::move(value);
}

void Watch::draw(WINDOW* w, int starty, int maxy, int delta_rows, bool title)
{
    int y = starty;

    wmove(w, y, 1);

    if (title && delta_rows <= 0) {
        wattron(w, A_BOLD);
        wprintw(w, "%s", name.c_str());
        wattroff(w, A_BOLD);

        if (++y == maxy)
            return;
    } else if (title) {
        delta_rows--;
    }

    for (Value& v : values) {
        if (delta_rows > 0) {
            delta_rows--;
            continue;
        }

        mvwprintw(w, y, 1, "%s: %s", v.name.c_str(), v.value.c_str());

        if (++y == maxy)
            return;
    }
}

int Watch::get_width() const
{
    return width;
}

int Watch::get_height() const
{
    return static_cast<int>(values.size());
}

const std::string& Watch::get_title() const
{
    return name;
}

}  // namespace nclogger
