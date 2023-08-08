#ifndef NCLOGGER_SCREEN_HPP
#define NCLOGGER_SCREEN_HPP

#include <cstdint>
#include <fstream>
#include <memory>
#include <mutex>
#include <nclogger/canvas.hpp>
#include <nclogger/text_block.hpp>
#include <nclogger/version.hpp>
#include <string>
#include <vector>

namespace nclogger {

#if 0

class Screen : public Canvas {
private:
    std::ofstream&                     logfile;
    std::vector<std::weak_ptr<Canvas>> composites;

public:
    Screen(std::string name, std::ofstream& logfile);

    void draw(int& delta_history) final;
    void add_composite_screen(std::weak_ptr<Canvas> composite);
    bool has_composite_screen(std::shared_ptr<Canvas>& composite);
    void log(LogLevel log_level, std::string message);
};

#endif

class Screen {
private:
    std::string name;

    std::ostream* logfile;

    std::vector<std::weak_ptr<Screen>> composites;

    TextBlock canvas;

    bool is_composite;

public:
    explicit Screen(std::string   name,
                    std::ostream* logfile      = nullptr,
                    bool          is_composite = false);

    [[nodiscard]] const std::string& get_name() const;

    void add_composite_screen(std::weak_ptr<Screen> composite);
    bool has_composite_screen(std::shared_ptr<Screen>& composite);

    void log(LogLevel log_level, const std::string& message);

    // Encapsulation of Canvas
    [[nodiscard]] inline std::size_t max_y_offset(const WINDOW* w) const
    {
        return canvas.max_y_offset(w);
    }

    inline void update_lines_index()
    {
        return canvas.update_lines_index();
    }

    inline void draw(WINDOW* w, std::size_t y_offset)
    {
        canvas.draw(w, y_offset);
    }
};

}  // namespace nclogger

#endif
