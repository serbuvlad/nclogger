///
/// \file to_string.hpp
/// \brief Convenience functions

#ifndef NCLOGGER_TO_STRING_HPP
#define NCLOGGER_TO_STRING_HPP

#include <nclogger/version.hpp>

#include <iomanip>
#include <sstream>
#include <string>

namespace nclogger {

///
/// \brief Convert value to string via std::stringstream.
///
/// Convenience function for creating a string via standard formatting.
///
/// \tparam T type of variable. must be supported by std::stringstream
/// \param val value to convert to a string
/// \return formatted string
template<typename T>
std::string to_string(T val)
{
    std::stringstream ss;

    ss << val;

    return std::move(ss.str());
}

///
/// \brief Convert value to string via std::stringstream with custom width.
///
/// Convenience function for creating a string with standard formatting. The
/// width of the value is set (via std::setw) before
///
/// \tparam T type of variable. must be supported by std::stringstream
/// \param val value to convert to a string
/// \param width the width of the value
/// \return formatted string
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
