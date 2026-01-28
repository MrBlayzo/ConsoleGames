#include <cmath>
#include <format>
#include "Game.h"

std::string RepeatableGenerator::generate(int number_length)
{
    std::string res = "";
    std::uniform_int_distribution<int> digit_dist(0, 9);
    for (int i = 0; i < number_length; ++i)
    {
        res += '0' + digit_dist(gen);
    }
    return res;
}

std::string UniqueGenerator::generate(int number_length)
{
    if (number_length > 10)
        throw std::runtime_error("Max number length is 10");

    std::string target = "0123456789";
    std::shuffle(target.begin(), target.end(), gen);
    target.resize(number_length);
    return target;
}

TargetNum::TargetNum(int number_length, GeneratorType type)
{
    if (type == GeneratorType::RepeatableDigits)
        generator = std::make_unique<RepeatableGenerator>();
    else if (type == GeneratorType::UniqueDigits)
        generator = std::make_unique<UniqueGenerator>();
    else
        throw std::runtime_error("Unknown generator type");

    generate(number_length);
}

std::string TargetNum::get()
{
    return target;
}
int TargetNum::length()
{
    return target.length();
}

NumType TargetNum::get_num_type(char c, int pos)
{
    if (pos<0 || pos >= target.length())
        throw std::runtime_error(std::format(
            "Digit pos {} out of range '{}'", pos, target));
    if (target[pos] == c)
        return NumType::Bull;
    if (digits.contains(c))
        return NumType::Cow;
    return NumType::None;
}

bool TargetNum::is_equal(std::string s)
{
    return target == s;
}
bool TargetNum::is_same_length(std::string s)
{
    return target.length() == s.length();
}

void TargetNum::generate(int number_length)
{
    if (number_length <= 0)
        throw std::runtime_error("Invalid length");

    target = generator->generate(number_length);
    digits.clear();
    for (auto &c : target)
    {
        digits.insert(c);
    }
}

BullsAndCows::BullsAndCows(GameParams params)
    : target(params.number_length, params.gen_type), params(params), engine()
{
}
void BullsAndCows::play()
{
    engine.clear();
    print_try(0);
    for (int i = 0; i < params.number_of_try; ++i)
    {

        print_mask();
        std::string new_number = engine.get();
        while (!target.is_same_length(new_number))
        {
            engine.print("Write number of correct length ", target.length(), "\n");
            new_number = engine.get();
        }
        if (params.hide_previous)
            engine.clear();
        print_try(i + 1);
        print_number(new_number);
        if (target.is_equal(new_number))
        {
            engine.print("You win!!!");
            return;
        }
    }
    engine.print("You lose!\nRight answer is ", target.get());
}

void BullsAndCows::print_mask()
{
    for (int i = 0; i < target.length(); ++i)
    {
        engine.print("_ ");
    }
    engine.print("\n");
}

void BullsAndCows::print_number(std::string number)
{
    for (int i = 0; i < number.length(); ++i)
    {
        NumType digit_type = target.get_num_type(number[i], i);
        if (digit_type == NumType::Bull)
            engine.print_color(ConsoleBkgColors::Red, number[i]);
        else if (digit_type == NumType::Cow)
            engine.print_color(ConsoleBkgColors::Yellow, number[i]);
        else
            engine.print(number[i]);
        engine.print(" ");
    }
    engine.print("\n");
}

void BullsAndCows::print_try(int current_try)
{
    engine.set_color(ConsoleTextColors::Red);
    for (int i = 0; i < current_try; ++i)
        engine.print("o");
    engine.set_color(ConsoleTextColors::Green);
    for (int i = current_try; i < params.number_of_try; ++i)
        engine.print("o");
    engine.reset_styles();
    engine.print("\n");
}