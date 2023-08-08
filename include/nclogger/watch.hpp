#ifndef NCLOGGER_WATCH_HPP
#define NCLOGGER_WATCH_HPP
#include <nclogger/version.hpp>
#include <string>
#include <vector>

namespace nclogger {

class Watch final {
private:
    struct Value {
        std::string name;
        std::string value;
        int         width;
    };

    std::string        name;
    std::vector<Value> values;

    int width = 0;

public:
    explicit Watch(std::string name);

    int register_value(std::string value_name, int value_width);

    void set_value(int index, std::string value);

    void draw(WINDOW* w, int starty, int mamxy, int delta_rows, bool title);

    [[nodiscard]] int                get_width() const;
    [[nodiscard]] int                get_height() const;
    [[nodiscard]] const std::string& get_title() const;
};

}  // namespace nclogger

#endif  //NCLOGGER_WATCH_HPP
