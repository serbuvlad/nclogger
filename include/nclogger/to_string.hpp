#ifndef NCLOGGER_TO_STRING_HPP
#define NCLOGGER_TO_STRING_HPP

#include <iomanip>
#include <sstream>
#include <string>

namespace nclogger {

template<typename T>
std::string to_string(T val)
{
    std::stringstream ss;

    ss << val;

    return std::move(ss.str());
}

template<typename T>
std::string to_string(T val, int width)
{
    std::stringstream ss;

    ss << std::setw(width) << val;

    return std::move(ss.str());
}

template<typename T>
std::string to_string(T val, void (*callback)(std::stringstream& ss))
{
    std::stringstream ss;

    callback(ss);

    ss << val;

    return std::move(ss.str());
}

template<typename T>
std::string to_string(T val, int width, void (*callback)(std::stringstream& ss))
{
    std::stringstream ss;

    callback(ss);

    ss << std::setw(width) << val;

    return std::move(ss.str());
}

}  // namespace nclogger

#endif  //NCLOGGER_TO_STRING_HPP
