#include "config/config.hpp"

Config::Config() {}
Config::Config(Config&& other) : properties(std::move(other.properties)) {}

Config::~Config() {}

Config& Config::operator=(Config&& other)
{
    this->properties = std::move(other.properties);
    return *this;
}

const ConfigValue* Config::operator[](const std::string& property) const
{
    return this->get(property);
}

const ConfigValue* Config::get(const std::string& property) const
{
    if(!this->contains(property))
        return nullptr;
    return this->properties.at(property).get();
}

void Config::put(const std::string& key, ConfigValue* value)
{
    this->properties[key] = std::unique_ptr<ConfigValue>(value);
}

bool Config::contains(const std::string& key) const
{
    return this->properties.count(key) != 0;
}
