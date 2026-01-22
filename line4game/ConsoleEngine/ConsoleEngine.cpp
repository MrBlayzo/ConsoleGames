#include <iostream>
#include "ConsoleEngine.h"

ConsoleEngine::ConsoleEngine()
{
    clear();
}

void ConsoleEngine::clear()
{
    std::cout << "\033[2J\033[H" << std::flush;
}

std::string ConsoleEngine::get()
{
    std::string input;
    std::getline(std::cin, input);
    std::cout << "\033[1A\033[2K\033[G" << std::flush;
    return input;
}