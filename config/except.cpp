#include "config/except.hpp"

#include <sstream>

std::string make_str()
{
    return "";
}

template <typename T, typename... Args>
std::string make_str(const T& arg, const Args&... args)
{
    std::stringstream ss;
    ss << arg;
    ss << make_str(args...);
    return ss.str();
}

ProgramException::ProgramException(const char* str) : std::runtime_error(str) {}
ProgramException::ProgramException(const std::string& str) : std::runtime_error(str) {}

ConfigException::ConfigException(const char* str) : ProgramException(str) {}
ConfigException::ConfigException(const std::string& str) : ProgramException(str) {}
ConfigException::ConfigException(size_t line, const char* str) : ProgramException(make_str("At line ", line, ": ", str)) {}
ConfigException::ConfigException(size_t line, const std::string& str) : ProgramException(make_str("At line ", line, ": ", str)) {}
