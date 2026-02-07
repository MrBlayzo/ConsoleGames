#pragma once
#include <random>

class RandomGenerator {
  public:
    static float rand();
    static int randint(int min, int max);

  private:
    static std::random_device rd;
    static std::mt19937 gen;
};
