#include <functional>
#include <nclogger/logger.hpp>
#include <nclogger/to_string.hpp>
#include <random>

using nclogger::LogLevel;
nclogger::Logger l;

int seed = static_cast<int>(time(nullptr));

std::thread* run_task(char c)
{
    return new std::thread([c]() {
        std::uniform_int_distribution<int> dice_distribution(1, 99);
        std::mt19937                       random_number_engine(
            seed);  // pseudorandom number generator
        auto dice_roller = std::bind(dice_distribution, random_number_engine);

        int screen      = l.request_screen(c, "Task " + nclogger::to_string(c));
        int watch       = l.request_watch(c, "Task " + nclogger::to_string(c));
        int based_watch = l.register_watch_value(watch, "Level", 3);
        int random_watch = l.register_watch_value(watch, "Random", 3);

        for (int i = 0; i < 9; i++) {
            int s = l.register_watch_value(
                watch,
                "Forever 0" + nclogger::to_string(c) + nclogger::to_string(i),
                1);
            l.set_watch_value(watch, s, nclogger::to_string(0));
        }

        for (int i = 0; i < 100; i++) {
            char msg[256];
            std::sprintf(msg, "info %d", i);
            l.log(screen, LogLevel::INFO, msg);
            l.set_watch_value(watch, based_watch, nclogger::to_string(i, 3));
            l.set_watch_value(watch,
                              random_watch,
                              nclogger::to_string(dice_roller(), 3));
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    });
}

int main()
{
    auto t1 = run_task('s');
    auto t2 = run_task('d');
    auto t3 = run_task('f');
    auto t4 = run_task('g');
    auto t5 = run_task('h');

    for (;;)
        ;
}
