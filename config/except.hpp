#ifndef _CONFIG_EXCEPT_HPP_
#define _CONFIG_EXCEPT_HPP_

#include <stdexcept>

class ProgramException : public std::runtime_error
{
    public:
        ProgramException(const char*);
        ProgramException(const std::string&);
        virtual ~ProgramException() = default;
};

class ConfigException : public ProgramException
{
    public:
        ConfigException(const char*);
        ConfigException(const std::string&);
        ConfigException(size_t, const char*);
        ConfigException(size_t, const std::string&);
        virtual ~ConfigException() = default;
};

#endif // _CONFIG_EXCEPT_HPP_
