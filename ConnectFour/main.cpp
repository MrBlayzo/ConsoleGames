#include <iostream>
#include <string>
#include <memory>
#include <stdexcept>
#include "Game.h"

struct GameParams
{
    int width = 7;
    int height = 6;
    std::string player1_spec = "human";
    std::string player2_spec = "human";
};

// Вспомогательная функция: разделить "key=value" на пару
std::pair<std::string, std::string> split_arg(const std::string &arg)
{
    size_t pos = arg.find('=');
    if (pos != std::string::npos)
    {
        return {arg.substr(0, pos), arg.substr(pos + 1)};
    }
    return {arg, ""};
}

GameParams get_params_from_args(int argc, char *argv[])
{
    GameParams params;

    for (int i = 1; i < argc; ++i)
    {
        auto [key, value] = split_arg(argv[i]);

        if (key == "-width" || key == "-w")
        {
            if (value.empty() && i + 1 < argc)
                value = argv[++i];
            if (value.empty())
            {
                std::cerr << "Missing value for -width\n";
                throw std::runtime_error("Missing value for -width");
            }
            try
            {
                if (auto new_width = std::stoi(value); new_width >= 4)
                    params.width = new_width;
                else
                    std::cerr << "Invalid number for " << key << ": " << value << "\n"
                              << "Use default value: " << params.width << "\n";
            }
            catch (...)
            {
                std::cerr << "Invalid number for " << key << ": " << value << "\n"
                          << "Use default value: " << params.width << "\n";
            }
        }
        else if (key == "-height" || key == "-h")
        {
            if (value.empty() && i + 1 < argc)
                value = argv[++i];
            if (value.empty())
            {
                std::cerr << "Missing value for -height\n";
                throw std::runtime_error("Missing value for -height");
            }
            try
            {
                if (auto new_height = std::stoi(value); new_height >= 4)
                    params.height = new_height;
                else
                    std::cerr << "Invalid number for " << key << ": " << value << "\n"
                              << "Use default value: " << params.height << "\n";
            }
            catch (...)
            {
                std::cerr << "Invalid number for " << key << ": " << value << "\n"
                          << "Use default value: " << params.height << "\n";
            }
        }
        else if (key == "-p1")
        {
            if (value.empty() && i + 1 < argc)
                value = argv[++i];
            if (!value.empty())
                params.player1_spec = value;
        }
        else if (key == "-p2")
        {
            if (value.empty() && i + 1 < argc)
                value = argv[++i];
            if (!value.empty())
                params.player2_spec = value;
        }
        else if (key == "-help")
        {
            std::cout << "Usage: ConnectFour [options]\n"
                      << "Options:\n"
                      << "  -width=N or -width N or -w=N or -w N\n"
                      << "  -height=N or -height N or -h=N or -h N\n"
                      << "  -p1=TYPE or -p1 TYPE   (e.g. human, minimax:4)\n"
                      << "  -p2=TYPE or -p2 TYPE\n";
            exit(0);
        }
    }
    return params;
}

ConnectFour make_game(GameParams params)
{
    return ConnectFour(params.width, params.height, player_from_string(params.player1_spec, Participant::player1),
                     player_from_string(params.player2_spec, Participant::player2));
}

int main(int argc, char *argv[])
{
    GameParams params = get_params_from_args(argc, argv);
    ConnectFour game = make_game(params);
    game.play();
    return 0;
}