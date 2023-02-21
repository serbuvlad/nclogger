#include "nclogger/canvas.hpp"

#include <ncurses.h>

#include <nclogger/knobs.hpp>
#include <utility>

namespace nclogger {

Canvas::Canvas(std::string name) : name(std::move(name)) {}

void Canvas::add_line(std::shared_ptr<const Line> line)
{
    if (lines.empty()) {
        lines.push_back(line);
        return;
    }

    for (auto it = lines.rbegin(); it != lines.rend(); it++) {
        if ((*it)->time_ns < line->time_ns) {
            lines.insert(it.base(), std::move(line));
            break;
        }
    }

    maybe_resize_lines();
}

void Canvas::draw(int& delta_history)
{
    draw(delta_history, true);
}

void Canvas::draw(int& delta_history, bool show_screen_name)
{
    int ysize = getmaxy(stdscr);
    int rows  = ysize - 2;

    std::size_t num_lines = lines.size();

    if (rows + delta_history > num_lines)
        delta_history = std::max(0, static_cast<int>(num_lines) - rows);

    if (num_lines <= rows) {
        for (int i = 0; i < num_lines; i++) {
            lines.at(i)->draw(i, show_screen_name);
        }
    } else {
        std::size_t start = num_lines - rows - delta_history;

        for (int i = 0; i < rows; i++) {
            lines.at(start + i)->draw(i, show_screen_name);
        }
    }

    attron(A_BOLD);
    mvprintw(ysize - 2, 0, "%s", name.c_str());

    if (delta_history > 0) {
        printw(" (-%d)", delta_history);
    }

    attroff(A_BOLD);

    move(ysize - 1, 0);
}

void Canvas::maybe_resize_lines()
{
    if (lines.size() >= screen_max_history + perf_screen_max_over_capacity) {
        lines.erase(lines.begin(),
                    lines.begin() + perf_screen_max_over_capacity);
    }
}

}  // namespace nclogger
