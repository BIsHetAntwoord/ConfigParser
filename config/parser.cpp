#include "config/parser.hpp"

#include <sstream>
#include <cstdint>

ConfigParser::ConfigParser(std::ifstream&& input) : input(std::move(input))
{
    if(!this->input)
            throw ConfigException("Failed to open configuration file");
}

ConfigParser::ConfigParser(const std::string& input_file) : ConfigParser(std::ifstream(input_file)) {}

ConfigParser::~ConfigParser() {}

char ConfigParser::read()
{
    if(this->unread_buffer.empty())
    {
        char c = this->input.get();
        if(c == '\n')
            ++this->lineno;
        return c;
    }
    else
    {
        char retval = this->unread_buffer.top();
        this->unread_buffer.pop();
        return retval;
    }
}

void ConfigParser::unread(char c)
{
    this->unread_buffer.push(c);
}

bool ConfigParser::eof()
{
    return this->unread_buffer.empty() && !this->input;
}

std::string ConfigParser::readId()
{
    char lookahead;
    do
    {
        lookahead = this->read();
    } while(!this->eof() && ConfigParser::isWhiteSpace(lookahead));

    if(this->eof())
        throw ConfigException(this->lineno, "Unexpected end of file while expecting start of identifier");

    if(!(std::isalpha(lookahead) || lookahead == '_'))
        throw ConfigException(this->lineno, "Unexpected character while expecting start of identifier");

    std::stringstream id_result;
    id_result << lookahead;

    lookahead = this->read();
    while(!this->eof() && (std::isalnum(lookahead) || lookahead == '_'))
    {
        id_result << lookahead;
        lookahead = this->read();
    }

    if(!this->eof())
        this->unread(lookahead);

    return id_result.str();
}

std::unique_ptr<ConfigValue> ConfigParser::readValue()
{
    char lookahead;
    do
    {
        lookahead = this->read();
    } while(!this->eof() && ConfigParser::isWhiteSpace(lookahead));

    if(ConfigParser::isNumberStart(lookahead) || lookahead == '-')
    {
        this->unread(lookahead);
        return this->readNumber();
    }
    else if(lookahead == '\"')
        return std::unique_ptr<ConfigValue>(new ConfigString(this->readString()));
    else
        throw ConfigException(this->lineno, "Unexpected character while expecting the start of a value");
}

std::unique_ptr<ConfigValue> ConfigParser::readNumber()
{
    char lookahead = this->read();
    if(this->eof())
        throw ConfigException(this->lineno, "Eof encountered while expecting start of number");
    if(!(ConfigParser::isNumberStart(lookahead) || lookahead == '-'))
        throw ConfigException(this->lineno, "Non-digit encountered while expecting start of number");

    std::stringstream result;
    if(lookahead == '0')
    {
        lookahead = this->read();
        if(this->eof())
            return std::unique_ptr<ConfigValue>(new ConfigInteger(0));
        //Hex mode
        if(lookahead == 'x' || lookahead == 'X')
        {
            lookahead = this->read();
            if(this->eof())
                throw ConfigException(this->lineno, "End of file after hex constant specifier");
            if(!ConfigParser::isHexDigit(lookahead))
                throw ConfigException(this->lineno, "No hex digit after hex constant specifier");

            while(!this->eof() && ConfigParser::isHexDigit(lookahead))
            {
                result << lookahead;
                lookahead = this->read();
            }
            if(!this->eof())
                this->unread(lookahead);

            int64_t result_i;
            result >> std::hex >> result_i;
            if(result.fail())
                throw ConfigException(this->lineno, "Could not convert hexadecimal value to integer");
            return std::unique_ptr<ConfigValue>(new ConfigInteger(result_i));
        }
        //Octal mode
        else if(ConfigParser::isOctDigit(lookahead))
        {
            while(!this->eof() && ConfigParser::isOctDigit(lookahead))
            {
                result << lookahead;
                lookahead = this->read();
            }
            if(!this->eof())
                this->unread(lookahead);

            int64_t result_i;
            result >> std::oct >> result_i;
            if(result.fail())
                throw ConfigException(this->lineno, "Could not convert octal value to integer");
            return std::unique_ptr<ConfigValue>(new ConfigInteger(result_i));
        }
    }

    if(lookahead == '-')
    {
        lookahead = this->read();
        if(!ConfigParser::isNumberStart(lookahead))
            throw ConfigException(this->lineno, "Expecting number after negative sign");
        result << "-";
    }

    bool found_dot = false;
    bool found_e = false;
    bool check_e = false;
    bool check_dot = false;

    //Default: decimal mode
    while(!this->eof() && (ConfigParser::isFloatCharacter(lookahead)))
    {
        if((lookahead == 'e' || lookahead == 'E') && check_dot) //An e cannot appear directly after a .
            throw ConfigException("Invalid number format: exponent directly after dot");

        check_dot = false;
        check_e = false;

        if(lookahead == '.' && (found_dot || found_e)) //A dot can appear only once before an e
            break;
        else if(lookahead == '.')
        {
            check_dot = true;
            found_dot = true;
        }
        else if((lookahead == 'e' || lookahead == 'E') && found_e) //Can only find one e
            break;
        else if(lookahead == 'e' || lookahead == 'E')
        {
            check_e = true;
            found_e = true;

            char next = this->read();
            if(next == '-')
            {
                result << lookahead;
                lookahead = next;
            }
            else
                this->unread(next);
        }

        result << lookahead;
        lookahead = this->read();
    }

    if(check_dot)
        throw ConfigException("Number constant ended with a dot");
    if(check_e)
        throw ConfigException("Number constant ended with an exponent");

    if(!this->eof())
        this->unread(lookahead);

    if(found_e || found_dot) //A float
    {
        double result_f;
        result >> result_f;
        return std::unique_ptr<ConfigValue>(new ConfigFloat(result_f));
    }
    else
    {
        int64_t result_i;
        result >> result_i;
        return std::unique_ptr<ConfigValue>(new ConfigInteger(result_i));
    }
}

std::string ConfigParser::readString()
{
    char lookahead = this->read();
    std::stringstream result;

    while(!this->eof() && lookahead != '\"')
    {
        if(lookahead == '\\')
        {
            lookahead = this->read();
            if(this->eof())
                throw ConfigException(this->lineno, "Eof encountered after escape sequence");
            switch(lookahead)
            {
                case '\"':
                    result << "\"";
                    break;
                case '\'':
                    result << "\'";
                    break;
                case 't':
                    result << "\t";
                    break;
                case 'r':
                    result << "\r";
                    break;
                case 'n':
                    result << "\n";
                    break;
                case '\\':
                    result << "\\";
                    break;
                default:
                    throw ConfigException(this->lineno, "Invalid escape sequence encountered");
            }
        }
        else
            result << lookahead;
        lookahead = this->read();
    }

    if(this->eof())
        throw ConfigException(this->lineno, "End of file encountered while parsing a string constant");
    return result.str();
}

Config ConfigParser::parse()
{
    Config result;

    std::string id_base;

    while(!this->eof())
    {
        char lookahead = this->read();
        if(this->eof())
            break;
        if(ConfigParser::isWhiteSpace(lookahead))
            continue;
        if(lookahead == '}')
        {
            if(groups.empty())
                throw ConfigException(this->lineno, "Encountered unmatched } outside of groups");
            groups.pop_back();
            continue;
        }
        this->unread(lookahead);

        std::string id = this->readId();

        do
        {
            lookahead = this->read();
        } while(!this->eof() && ConfigParser::isWhiteSpace(lookahead));

        if(this->eof())
            throw ConfigException(this->lineno, "Unexpected end of file after id");

        switch(lookahead)
        {
            case ':':
            {
                std::unique_ptr<ConfigValue> value = this->readValue();
                result.put(this->makeId(id_base + id), value.release());
                id_base.clear();
            }
                break;
            case '{':
                this->groups.push_back(id_base + id);
                id_base.clear();
                break;
            case '.':
                id_base += id + ".";
                break;
            default:
                throw ConfigException(this->lineno, "Unexpected token in input after id");
        }
    }

    if(!this->groups.empty())
        throw ConfigException(this->lineno, "Unclosed { at end of file");

    return result;
}

std::string ConfigParser::makeId(const std::string& base_id) const
{
    std::stringstream idstr_maker;
    bool first = true;
    for(auto& name : this->groups)
    {
        if(first)
            first = false;
        else
            idstr_maker << ".";

        idstr_maker << name;
    }

    if(!first)
        idstr_maker << ".";
    idstr_maker << base_id;
    return idstr_maker.str();
}

bool ConfigParser::isWhiteSpace(char c)
{
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

bool ConfigParser::isHexDigit(char c)
{
    return (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') ||
        (c >= '0' && c <= '9');
}

bool ConfigParser::isOctDigit(char c)
{
    return (c >= '0' && c <= '7');
}

bool ConfigParser::isFloatCharacter(char c)
{
    return (c >= '0' && c <= '9') || c == 'e' || c == 'E' ||
        c == '.';
}

bool ConfigParser::isNumberStart(char c)
{
    return (c >= '0' && c <= '9') || c == '.';
}
