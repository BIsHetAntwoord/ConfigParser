#ifndef _CONFIG_CONFIG_HPP_
#define _CONFIG_CONFIG_HPP_

#include <map>
#include <memory>

#include "config/values.hpp"

class Config
{
    private:
        std::map<std::string, std::unique_ptr<ConfigValue>> properties;
    public:
        Config();
        Config(Config&&);
        Config(const Config&) = delete;
        ~Config();

        Config& operator=(Config&&);
        Config& operator=(const Config&) = delete;

        using iterator = decltype(properties)::iterator;
        using const_iterator = decltype(properties)::const_iterator;

        const ConfigValue* operator[](const std::string&) const;
        const ConfigValue* get(const std::string&) const;
        void put(const std::string&, ConfigValue*);
        bool contains(const std::string&) const;

        inline iterator begin()
        {
            return properties.begin();
        }
        inline const_iterator cbegin() const
        {
            return properties.cbegin();
        }
        inline iterator end()
        {
            return properties.end();
        }
        inline const_iterator cend() const
        {
            return properties.cend();
        }
        inline const_iterator begin() const
        {
            return properties.begin();
        }
        inline const_iterator end() const
        {
            return properties.end();
        }
};

#endif
