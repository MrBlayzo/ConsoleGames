#include <gtest/gtest.h>

#include <iostream>
#include <sstream>

#include "ConsoleEngine.h"

class ConsoleEngineTest : public ::testing::Test {
  protected:
    std::istringstream in{""};
    std::ostringstream out;
    std::unique_ptr<ConsoleEngine> engine;
    void SetUp() override { engine = std::make_unique<ConsoleEngine>(in, out); }
    void TearDown() override {}

    void clear_out() {
        out.str("");
        out.clear();
    }
};

TEST_F(ConsoleEngineTest, Clear) {
    engine->clear();
    EXPECT_EQ(out.str(), "\033[2J\033[H");
}

TEST_F(ConsoleEngineTest, SetCursorZero) {
    engine->set_cursor_to_zero();
    EXPECT_EQ(out.str(), "\033[H");
}

TEST_F(ConsoleEngineTest, SetCursor) {
    engine->set_cursor_to_pos(1, 2);
    EXPECT_EQ(out.str(), "\033[3;2H");
}

TEST_F(ConsoleEngineTest, Print) {
    engine->print("Hello ", "world", "\n");
    EXPECT_EQ(out.str(), "Hello world\n");
}

TEST(ConsoleEnumsTest, ConsoleStyleValues) {
    EXPECT_EQ(static_cast<int>(ConsoleStyle::Reset), 0);
    EXPECT_EQ(static_cast<int>(ConsoleStyle::Bold), 1);
    EXPECT_EQ(static_cast<int>(ConsoleStyle::Underline), 4);
}

TEST(ConsoleEnumsTest, TextColorValues) {
    EXPECT_EQ(static_cast<int>(ConsoleTextColors::Black), 30);
    EXPECT_EQ(static_cast<int>(ConsoleTextColors::Red), 31);
    EXPECT_EQ(static_cast<int>(ConsoleTextColors::Green), 32);
    EXPECT_EQ(static_cast<int>(ConsoleTextColors::Yellow), 33);
    EXPECT_EQ(static_cast<int>(ConsoleTextColors::Blue), 34);
    EXPECT_EQ(static_cast<int>(ConsoleTextColors::Magenta), 35);
    EXPECT_EQ(static_cast<int>(ConsoleTextColors::Cyan), 36);
    EXPECT_EQ(static_cast<int>(ConsoleTextColors::White), 37);
}

TEST(ConsoleEnumsTest, BackgroundColorValues) {
    EXPECT_EQ(static_cast<int>(ConsoleBkgColors::Black), 40);
    EXPECT_EQ(static_cast<int>(ConsoleBkgColors::Red), 41);
    EXPECT_EQ(static_cast<int>(ConsoleBkgColors::Green), 42);
    EXPECT_EQ(static_cast<int>(ConsoleBkgColors::Yellow), 43);
    EXPECT_EQ(static_cast<int>(ConsoleBkgColors::Blue), 44);
    EXPECT_EQ(static_cast<int>(ConsoleBkgColors::Magenta), 45);
    EXPECT_EQ(static_cast<int>(ConsoleBkgColors::Cyan), 46);
    EXPECT_EQ(static_cast<int>(ConsoleBkgColors::White), 47);
}

TEST_F(ConsoleEngineTest, ResetStyles) {
    engine->reset_styles();
    EXPECT_EQ(out.str(), "\033[0m");
}

TEST_F(ConsoleEngineTest, SetStyle) {
    engine->set_style(ConsoleStyle::Bold);
    EXPECT_EQ(out.str(), "\033[1m");
    clear_out();
    engine->set_style(ConsoleStyle::Underline);
    EXPECT_EQ(out.str(), "\033[4m");
    clear_out();
}

TEST_F(ConsoleEngineTest, SetColor) {
    engine->set_color(ConsoleTextColors::Black);
    EXPECT_EQ(out.str(), "\033[30m");
    clear_out();
    engine->set_color(ConsoleBkgColors::Black);
    EXPECT_EQ(out.str(), "\033[40m");
    clear_out();
    engine->set_color(ConsoleTextColors::Magenta, ConsoleBkgColors::Cyan);
    EXPECT_EQ(out.str(), "\033[35;46m");
    clear_out();
}

TEST_F(ConsoleEngineTest, PrintColor) {
    engine->print_color(ConsoleTextColors::Red, "Hello");
    EXPECT_EQ(out.str(), "\033[31mHello\033[0m");
    clear_out();
    engine->print_color(ConsoleBkgColors::Green, "Hello");
    EXPECT_EQ(out.str(), "\033[42mHello\033[0m");
    clear_out();
    engine->print_color(ConsoleTextColors::Yellow, ConsoleBkgColors::Blue,
                        "Hello");
    EXPECT_EQ(out.str(), "\033[33;44mHello\033[0m");
    clear_out();
}

TEST_F(ConsoleEngineTest, Get) {
    in.str("aadd\nwwss");
    EXPECT_EQ(engine->get(), "aadd");
    EXPECT_EQ(engine->get(), "wwss");
}