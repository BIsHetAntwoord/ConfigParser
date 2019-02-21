#ifndef _CONFIG_WRITER_HPP_
#define _CONFIG_WRITER_HPP_

#include <fstream>

class Config;

class ConfigWriter
{
    private:
        std::ofstream output;
    public:
        ConfigWriter(const std::string&);
        ~ConfigWriter();

        void write(const Config&);
};

#endif // _CONFIG_WRITER_HPP_
