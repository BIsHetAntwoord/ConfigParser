#ifndef _CONFIG_PARSER_HPP_
#define _CONFIG_PARSER_HPP_

#include <fstream>
#include <stack>
#include <vector>
#include <string>
#include <memory>

#include "config/except.hpp"
#include "config/config.hpp"

class ConfigParser
{
    private:
        std::ifstream input;
        std::vector<std::string> groups;
        std::stack<char> unread_buffer;
        size_t lineno = 1;

        void unread(char);
        char read();
        bool eof();

        std::string readId();
        std::unique_ptr<ConfigValue> readValue();
        std::unique_ptr<ConfigValue> readNumber();
        std::string readString();

        std::string makeId(const std::string&) const;

        static bool isWhiteSpace(char);
        static bool isHexDigit(char);
        static bool isOctDigit(char);
        static bool isFloatCharacter(char);
        static bool isNumberStart(char);
    public:
        ConfigParser(std::ifstream&&);
        ConfigParser(const std::string&);

        ~ConfigParser();

        Config parse();
};

#endif // _CONFIG_PARSER_HPP_
