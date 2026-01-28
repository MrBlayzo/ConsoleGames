#pragma once
#include <string>
#include <iostream>
#ifdef _WIN32
#include <windows.h>
#endif

enum class ConsoleStyle
{
    Reset = 0,
    Bold = 1,
    Underline = 4,
};
enum class ConsoleTextColors
{
    Black = 30,
    Red = 31,
    Green = 32,
    Yellow = 33,
    Blue = 34,
    Magenta = 35,
    Cyan = 36,
    White = 37,
};
enum class ConsoleBkgColors
{
    Black = 40,
    Red = 41,
    Green = 42,
    Yellow = 43,
    Blue = 44,
    Magenta = 45,
    Cyan = 46,
    White = 47,
};

class ConsoleEngine
{
public:
    ConsoleEngine();
    ~ConsoleEngine();
    void clear();
    template <typename... Args>
    void print(Args... args)
    {
        ((std::cout << args), ...);
    };
    template <typename... Args>
    void print_color(ConsoleTextColors text_color, ConsoleBkgColors background_color, Args... args)
    {
        set_color(text_color);
        set_color(background_color);
        print(args...);
        reset_styles();
    };
    template <typename... Args>
    void print_color(ConsoleTextColors text_color, Args... args)
    {
        set_color(text_color);
        print(args...);
        reset_styles();
    };
    template <typename... Args>
    void print_color(ConsoleBkgColors background_color, Args... args)
    {
        set_color(background_color);
        print(args...);
        reset_styles();
    };
    void reset_styles();
    void set_style(ConsoleStyle style);
    void set_color(ConsoleTextColors text_color);
    void set_color(ConsoleBkgColors background_color);
    std::string get();

private:
    void enableAnsiColors() {
#ifdef _WIN32
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut == INVALID_HANDLE_VALUE) return;

        DWORD dwMode = 0;
        if (!GetConsoleMode(hOut, &dwMode)) return;

        // Включаем поддержку ANSI escape-кодов
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, dwMode);
#endif
    }
};