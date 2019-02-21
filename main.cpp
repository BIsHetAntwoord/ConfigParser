#include "config/parser.hpp"
#include "config/writer.hpp"

#include <iostream>

int main()
{
    try
    {
        ConfigParser parser("tests/config.txt");
        Config result = parser.parse();

        int64_t value = result.contains("d.e.f") ? result.get("d.e.f")->getInteger() : 0;
        result.put("d.e.f", new ConfigInteger(value+1));

        ConfigWriter writer("tests/config.txt");
        writer.write(result);

    }
    catch(const std::runtime_error& err)
    {
        std::cerr << "Exception: " << err.what() << std::endl;
    }

    return 0;
}
