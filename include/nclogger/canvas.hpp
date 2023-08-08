#ifndef NCLOGGER_CANVAS_HPP
#define NCLOGGER_CANVAS_HPP

#include <nclogger/line.hpp>
#include <nclogger/version.hpp>

namespace nclogger {

class Canvas {
private:
    friend class Logger;
    friend class Screen;

    Lines       lines;
    std::string name;

public:
    [[nodiscard]] const std::string& get_name() const;

public:
    explicit Canvas(std::string name);

    void         add_line(std::shared_ptr<const Line> line);
    virtual void draw(int& delta_history);
    void         draw(int& delta_history, bool show_screen_name);
    void         maybe_resize_lines();
};

}  // namespace nclogger

#endif  //NCLOGGER_CANVAS_HPP
