#include <ncurses.h>

#include <algorithm>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <nclogger/canvas.hpp>
#include <nclogger/knobs.hpp>
#include <nclogger/screen.hpp>
#include <ostream>

namespace nclogger {

Screen::Screen(std::string name, std::ofstream& logfile)
    : Canvas(std::move(name)), logfile(logfile)
{
}

void Screen::log(LogLevel log_level, std::string message)
{
    std::shared_ptr<const Line> line =
        Line::create(std::move(message), name, log_level);

    line->write(logfile);

    add_line(line);

    for (auto& composite : composites) {
        auto c = composite.lock();

        if (c) {
            c->add_line(line);
        }
    }
}

void Screen::draw(int& delta_history)
{
    Canvas::draw(delta_history, false);
}

void Screen::add_composite_screen(std::weak_ptr<Canvas> composite)
{
    composites.push_back(std::move(composite));
}

bool Screen::has_composite_screen(std::shared_ptr<Canvas>& composite)
{
    return std::any_of(composites.cbegin(),
                       composites.cend(),
                       [composite](const std::weak_ptr<Canvas>& c) {
                           return c.lock() == composite;
                       });
}

}  // namespace nclogger
