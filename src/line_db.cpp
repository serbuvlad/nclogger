#include <nclogger/line_db.hpp>

namespace nclogger {

void LineDB::run_on_lines(const std::unordered_set<index_t>& screens,
                          LogLevel min_log_level,
                          run_on_lines_callback callback)
{
    std::vector<ScreenLogBucket::reverse_iterator> bucket_its;
    std::vector<ScreenLogBucket::reverse_iterator> bucket_its_end;
    
    for (index_t screen_id : screens) {
        for (ScreenLogBucket& screen_bucket : lines.at(screen_id)) {
            bucket_its.push_back(screen_bucket.rbegin());
            bucket_its_end.push_back(screen_bucket.rend());
        }
    }

    bool done = false;
    while (!done) {
        ScreenLogBucket::reverse_iterator* latest_line_candidate = nullptr;
        auto latest_time_ns = std::numeric_limits<std::int64_t>::lowest();

        for (std::size_t i = 0; i < bucket_its.size(); ++i) {
            std::int64_t time_ns;

            if (bucket_its[i] != bucket_its_end[i] &&
                (time_ns = (*bucket_its[i])->get_time_ns()) > latest_time_ns) {
                    
                latest_line_candidate = &bucket_its[i];
                latest_time_ns = time_ns;
            }
        }

        if (latest_line_candidate != nullptr) {
            auto& latest_line_it = *latest_line_candidate;
            auto& latest_line = *latest_line_it;

            ++latest_line_it;
            
            if (latest_line->get_log_level() >= min_log_level)
                done = callback(latest_line);
        } else {
            done = true;
        }
    }
}

}
