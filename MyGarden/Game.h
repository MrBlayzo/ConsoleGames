#pragma once
#include <memory>
#include <random>
#include <vector>

#include "ConsoleEngine.h"
#include "GameObjects.h"

class Generator {
  public:
    static float rand();
    static int randint(int min, int max);

  private:
    static std::random_device rd;
    static std::mt19937 gen;
};

class Cell {
  public:
    std::unique_ptr<Object> content;
    bool is_selected;

    Cell(int x, int y, std::unique_ptr<Object> content);
    void draw(ConsoleEngine& engine);

  private:
    int pos_x;
    int pos_y;
};

class Map {
  public:
    Map(int width, int height);
    void update();

  private:
    int width;
    int height;
    std::vector<std::vector<Cell>> map;
    ConsoleEngine engine;
    int cursor_x;
    int cursor_y;

    void generate();
    void generate_lakes();
    void generate_rivers();
    void generate_objects();
};

class MyGarden {
  public:
    MyGarden(int width = 100, int height = 20);
    void play();

  private:
    Map map;
};