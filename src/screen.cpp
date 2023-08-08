#include <algorithm>
#include <cassert>
#include <chrono>
#include <ctime>
#include <nclogger/canvas.hpp>
#include <nclogger/screen.hpp>
#include <nclogger/to_string.hpp>
#include <ostream>

namespace nclogger {

static inline std::string log_level_string(LogLevel log_level)
{
    switch (log_level) {
    case LogLevel::INFO: return " {INFO}";
    case LogLevel::WARN: return " {WARN}";
    case LogLevel::ERROR: return "{ERROR}";
    case LogLevel::FATAL: return "{FATAL}";
    }

    assert(false);
}

Screen::Screen(std::string name, std::ostream* logfile, bool is_composite)
    : name(std::move(name)), logfile(logfile), is_composite(is_composite)
{
}

void Screen::add_composite_screen(std::weak_ptr<Screen> composite)
{
    composites.push_back(std::move(composite));
}

bool Screen::has_composite_screen(std::shared_ptr<Screen>& composite)
{
    return std::any_of(composites.cbegin(),
                       composites.cend(),
                       [composite](const std::weak_ptr<Screen>& c) {
                           return c.lock() == composite;
                       });
}

static inline std::string format_time(std::uint64_t time_ms)
{
    auto time_s = static_cast<std::time_t>(time_ms / 1000);

    char str_buf[100];
    tm   tm_buf{};

    strftime(str_buf, 100, "[%F %T.", localtime_r(&time_s, &tm_buf));

    return str_buf + to_string(time_ms % 1000) + "]";
}

static void add_line_to_canvas(TextBlock&         text_block,
                               const std::string* screen_name,
                               const std::string& time,
                               LogLevel           log_level,
                               const std::string& message)
{
    if (screen_name != nullptr)
        text_block.add_string("(" + *screen_name + ") ");

    text_block.add_string(time + " ");
    text_block.add_string(message + "\n",
                          A_NORMAL,
                          static_cast<short>(log_level));

    text_block.update_lines_index();
}

static void add_line_to_file(std::ostream&      logfile,
                             const std::string& screen_name,
                             const std::string& time,
                             LogLevel           log_level,
                             const std::string& message)
{
    logfile << "(" + screen_name + ") " << time + " "
            << log_level_string(log_level) << message + "\n";
}

void Screen::log(LogLevel log_level, const std::string& message)
{
    assert(!is_composite);

    std::uint64_t time_ms =
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch())
            .count();

    std::string time_string = format_time(time_ms);

    add_line_to_canvas(canvas, nullptr, time_string, log_level, message);

    for (std::weak_ptr<Screen>& composite_weak_ptr : composites) {
        std::shared_ptr<Screen> composite = composite_weak_ptr.lock();

        add_line_to_canvas(composite->canvas,
                           &name,
                           time_string,
                           log_level,
                           message);
    }

    if (logfile != nullptr)
        add_line_to_file(*logfile, name, time_string, log_level, message);
}

const std::string& Screen::get_name() const
{
    return name;
}

}  // namespace nclogger
