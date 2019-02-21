#ifndef _CONFIG_VALUES_HPP_
#define _CONFIG_VALUES_HPP_

#include <string>
#include <cstdint>
#include <sstream>

class ConfigValue
{
    public:
        virtual ~ConfigValue() = default;

        virtual std::string getString() const;
        virtual int64_t getInteger() const;
        virtual double getFloat() const;
        virtual std::string fileFormat() const = 0;
};

template <typename T>
class ConfigValueType : public ConfigValue
{
    private:
        T value;
    public:
        ConfigValueType(const T&);
        virtual ~ConfigValueType() = default;

        virtual std::string getString() const;
        virtual int64_t getInteger() const;
        virtual double getFloat() const;
        virtual std::string fileFormat() const;
};

typedef ConfigValueType<std::string> ConfigString;
typedef ConfigValueType<int64_t> ConfigInteger;
typedef ConfigValueType<double> ConfigFloat;

//Implementations
template <typename T>
ConfigValueType<T>::ConfigValueType(const T& value) : value(value) {}

template <typename T>
std::string ConfigValueType<T>::getString() const
{
    std::stringstream ss;
    ss << this->value;
    return ss.str();
}

template<>
int64_t ConfigValueType<std::string>::getInteger() const;
template<>
double ConfigValueType<std::string>::getFloat() const;

template <typename T>
int64_t ConfigValueType<T>::getInteger() const
{
    return static_cast<int64_t>(this->value);
}

template <typename T>
double ConfigValueType<T>::getFloat() const
{
    return static_cast<double>(this->value);
}

template<>
std::string ConfigValueType<std::string>::fileFormat() const;
template<>
std::string ConfigValueType<int64_t>::fileFormat() const;
template<>
std::string ConfigValueType<double>::fileFormat() const;

#endif // _CONFIG_VALUES_HPP_
