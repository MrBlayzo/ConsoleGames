#include <iostream>
#include "ConsoleEngine.h"

ConsoleEngine::ConsoleEngine()
{
    enableAnsiColors();
}
ConsoleEngine::~ConsoleEngine()
{
    reset_styles();
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

void ConsoleEngine::reset_styles()
{
    std::cout << "\033[" << static_cast<int>(ConsoleStyle::Reset) << "m";
}
void ConsoleEngine::set_style(ConsoleStyle style)
{
    std::cout << "\033[" << static_cast<int>(style) << "m";
}
void ConsoleEngine::set_color(ConsoleTextColors text_color)
{
    std::cout << "\033[" << static_cast<int>(text_color) << "m";
}
void ConsoleEngine::set_color(ConsoleBkgColors background_color)
{
    std::cout << "\033[" << static_cast<int>(background_color) << "m";
}