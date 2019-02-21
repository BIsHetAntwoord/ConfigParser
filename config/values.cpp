#include "config/values.hpp"
#include "config/except.hpp"

std::string ConfigValue::getString() const
{
    throw ConfigException("Tried to read config value as a string, which was not supported");
}

int64_t ConfigValue::getInteger() const
{
    throw ConfigException("Tried to read config value as an integer, which was not supported");
}

double ConfigValue::getFloat() const
{
    throw ConfigException("Tried to read config value as a float, which was not supported");
}

template<>
int64_t ConfigValueType<std::string>::getInteger() const
{
    throw ConfigException("Tried to read a string configuration value as an integer, which was not supported");
}

template<>
double ConfigValueType<std::string>::getFloat() const
{
    throw ConfigException("Tried to read a string configuration value as a float, which was not supported");
}

template<>
std::string ConfigValueType<std::string>::fileFormat() const
{
    std::string value = this->getString();

    std::stringstream result;
    result << "\"";

    for(char c : value)
    {
        switch(c)
        {
            case '\"':
                result << "\\\"";
                break;
            case '\'':
                result << "\\\'";
                break;
            case '\t':
                result << "\\t";
                break;
            case '\n':
                result << "\\n";
                break;
            case '\r':
                result << "\\r";
                break;
            case '\\':
                result << "\\\\";
                break;
            default:
                result << c;
                break;
        }
    }
    result << "\"";
    return result.str();
}

template<>
std::string ConfigValueType<double>::fileFormat() const
{
    std::string retval = this->getString();

    //If in integer format
    if(retval.find('.') == std::string::npos && retval.find('e') == std::string::npos &&
       retval.find('E') == std::string::npos)
        return retval + ".0";
    return retval;
}

template<>
std::string ConfigValueType<int64_t>::fileFormat() const
{
    return this->getString();
}
