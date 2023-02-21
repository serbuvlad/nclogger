#ifndef NCLOGGER_SCREEN_HPP
#define NCLOGGER_SCREEN_HPP

#include <cstdint>
#include <fstream>
#include <memory>
#include <mutex>
#include <nclogger/canvas.hpp>
#include <string>
#include <vector>

namespace nclogger {

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

}  // namespace nclogger

#endif
