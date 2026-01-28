#pragma once
#include <memory>
#include <vector>
#include <random>
#include <unordered_set>
#include "ConsoleEngine.h"

enum class NumType
{
    Bull,
    Cow,
    None
};

enum class GeneratorType
{
    RepeatableDigits,
    UniqueDigits,
};

class NumGenerator{
public:
    virtual ~NumGenerator() = default;
    virtual std::string generate(int number_length) = 0;

protected:
    std::random_device rd;
    std::mt19937 gen{rd()};
};

class RepeatableGenerator: public NumGenerator{
public:
    std::string generate(int number_length) override;
};

class UniqueGenerator: public NumGenerator{
public:
    std::string generate(int number_length) override;
};

class TargetNum
{
public:
    TargetNum(int number_length, GeneratorType type = GeneratorType::UniqueDigits);
    void generate(int number_length);
    std::string get();
    NumType get_num_type(char c, int pos);
    bool is_equal(std::string s);
    bool is_same_length(std::string s);
    int length();

private:
    std::string target;
    std::unordered_set<char> digits;
    std::unique_ptr<NumGenerator> generator;
};

struct GameParams
{
    int number_length = 4;
    int number_of_try = 6;
    GeneratorType gen_type = GeneratorType::UniqueDigits;
    bool hide_previous = true;
};

class BullsAndCows
{
public:
    BullsAndCows(GameParams params = GameParams{});
    void play();

private:
    TargetNum target;
    GameParams params;
    ConsoleEngine engine;

    void print_mask();
    void print_number(std::string number);
    void print_try(int current_try);
};