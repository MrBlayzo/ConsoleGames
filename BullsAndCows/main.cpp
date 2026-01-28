#include <iostream>
#include <string>
#include <memory>
#include <stdexcept>
#include "Game.h"

std::pair<std::string, std::string> split_arg(const std::string &arg)
{
    size_t pos = arg.find('=');
    if (pos != std::string::npos)
    {
        return {arg.substr(0, pos), arg.substr(pos + 1)};
    }
    return {arg, ""};
}

GeneratorType generator_type_from_string(std::string s)
{
    if (s == "repeat")
        return GeneratorType::RepeatableDigits;
    return GeneratorType::UniqueDigits;
}

GameParams get_params_from_args(int argc, char *argv[])
{
    GameParams params;

    for (int i = 1; i < argc; ++i)
    {
        auto [key, value] = split_arg(argv[i]);

        if (key == "-l")
        {
            if (value.empty() && i + 1 < argc)
                value = argv[++i];
            if (value.empty())
            {
                std::cerr << "Missing value for -l\n";
                throw std::runtime_error("Missing value for -l");
            }
            try
            {
                params.number_length = std::stoi(value);
            }
            catch (...)
            {
                std::cerr << "Invalid number for " << key << ": " << value << "\n"
                          << "Use default value: " << params.number_length << "\n";
            }
        }
        else if (key == "-t")
        {
            if (value.empty() && i + 1 < argc)
                value = argv[++i];
            if (value.empty())
            {
                std::cerr << "Missing value for -t\n";
                throw std::runtime_error("Missing value for -t");
            }
            try
            {
                params.number_of_try = std::stoi(value);
            }
            catch (...)
            {
                std::cerr << "Invalid number for " << key << ": " << value << "\n"
                          << "Use default value: " << params.number_of_try << "\n";
            }
        }
        else if (key == "-g")
        {
            if (value.empty() && i + 1 < argc)
                value = argv[++i];
            if (!value.empty())
                params.gen_type = generator_type_from_string(value);
        }
        else if (key == "-h")
        {
            if (value.empty() && i + 1 < argc)
                value = argv[++i];
            if (!value.empty())
            {
                if (value.find('t') != std::string::npos)
                {
                    params.hide_previous = true;
                }
                else
                {
                    params.hide_previous = false;
                }
            }
        }
        else if (key == "-help")
        {
            std::cout << "Usage: BullsAndCows [options]\n"
                      << "Options:\n"
                      << "  -l=N\n"
                      << "  -t=N\n"
                      << "  -g=TYPE (e.g. repeat, unique)\n"
                      << "  -h=false/true\n";
            exit(0);
        }
    }
    return params;
}

int main(int argc, char *argv[])
{
    BullsAndCows game = BullsAndCows(get_params_from_args(argc, argv));
    game.play();
    return 0;
}