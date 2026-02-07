#include "RandomGenerator.h"

std::random_device RandomGenerator::rd;
std::mt19937 RandomGenerator::gen{RandomGenerator::rd()};

float RandomGenerator::rand() {
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    return dist(gen);
}

int RandomGenerator::randint(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(gen);
}