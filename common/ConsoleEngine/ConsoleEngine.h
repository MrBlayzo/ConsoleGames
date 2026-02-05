#pragma once
#include <iostream>
#include <string>
#include <cstdint>

#ifdef _WIN32
#define NOMINMAX
#include <conio.h>
#include <windows.h>
inline bool uni_kbhit() { return _kbhit() != 0; }
inline char uni_getch() { return _getch(); }
#else
#include <stdio.h>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>

inline bool uni_kbhit() {
    int ch = -1;
    struct termios old_termios, new_termios;
    tcgetattr(STDIN_FILENO, &old_termios);
    new_termios = old_termios;
    new_termios.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(STDIN_FILENO, &read_fds);
    struct timeval timeout = {0, 0};  // неблокирующий
    int ready = select(STDIN_FILENO + 1, &read_fds, nullptr, nullptr, &timeout);
    if (ready > 0 && FD_ISSET(STDIN_FILENO, &read_fds)) {
        ch = getchar();
        ungetc(ch, stdin);  // вернуть символ обратно
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &old_termios);
    return ch != -1;
}

inline char uni_getch() {
    struct termios old_termios, new_termios;
    tcgetattr(STDIN_FILENO, &old_termios);
    new_termios = old_termios;
    new_termios.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
    char ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &old_termios);
    return ch;
}
#endif

enum class ConsoleStyle {
    Reset = 0,
    Bold = 1,
    Underline = 4,
    Inverse = 7,
};
enum class ConsoleTextColors {
    Black = 30,
    Red = 31,
    Green = 32,
    Yellow = 33,
    Blue = 34,
    Magenta = 35,
    Cyan = 36,
    White = 37,
};
enum class ConsoleBkgColors {
    Black = 40,
    Red = 41,
    Green = 42,
    Yellow = 43,
    Blue = 44,
    Magenta = 45,
    Cyan = 46,
    White = 47,
};

struct Color256 {
    uint8_t id;
    constexpr Color256(int i) : id(static_cast<uint8_t>(i)) {};
};

namespace Colors256{
    constexpr Color256 Black{0};
    constexpr Color256 Red{196};
    constexpr Color256 Green{46};
    constexpr Color256 Yellow{226};
    constexpr Color256 Blue{21};
    constexpr Color256 Magenta{201};
    constexpr Color256 Cyan{51};
    constexpr Color256 White{231};
    constexpr Color256 Pink{198};
    constexpr Color256 Orange{208};
    constexpr Color256 Gray20{235};
    constexpr Color256 Gray50{240};
    constexpr Color256 Gray80{248};
    constexpr Color256 DarkGreen{22};
    constexpr Color256 GrayBrown{101};
    constexpr Color256 LightBrown{136};
    constexpr Color256 OrangeBrown{130};
    constexpr Color256 Purple{35};
}

class ConsoleEngine {
  public:
    ConsoleEngine();
    ConsoleEngine(std::istream& in, std::ostream& out);
    ~ConsoleEngine();
    void clear();
    void set_cursor_to_zero();
    void set_cursor_to_pos(int x, int y);
    template <typename... Args>
    void print(Args... args) {
        ((cout_ << args), ...);
    };
    template <typename... Args>
    void print_color(ConsoleTextColors text_color,
                     ConsoleBkgColors background_color, Args... args) {
        set_color(text_color, background_color);
        print(args...);
        reset_styles();
    };
    template <typename... Args>
    void print_color(ConsoleTextColors text_color, Args... args) {
        set_color(text_color);
        print(args...);
        reset_styles();
    };
    template <typename... Args>
    void print_color(ConsoleBkgColors background_color, Args... args) {
        set_color(background_color);
        print(args...);
        reset_styles();
    };
    template <typename... Args>
    void print_color(Color256 text_color, Args... args) {
        set_text_color(text_color);
        print(args...);
        reset_styles();
    };
    template <typename... Args>
    void print_color(Color256 text_color, Color256 background_color, Args... args) {
        set_text_color(text_color);
        set_background_color(background_color);
        print(args...);
        reset_styles();
    };
    void reset_styles();
    void set_style(ConsoleStyle style);
    void set_color(ConsoleTextColors text_color);
    void set_color(ConsoleBkgColors background_color);
    void set_color(ConsoleTextColors text_color,
                   ConsoleBkgColors background_color);
    void set_text_color(Color256 color);
    void set_background_color(Color256 color);
    std::string get();
    char get_no_wait();
    bool key_pressed(char key);

  private:
    std::istream& cin_;
    std::ostream& cout_;

    void flush_input_buffer();
    void enableAnsiColors();
};