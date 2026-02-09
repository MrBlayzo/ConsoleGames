#pragma once
#include <deque>
#include <memory>
#include <optional>
#include <thread>
#include <vector>

#include "ConsoleEngine.h"
#include "GameObjects.h"
#include "RandomGenerator.h"

class Cell {
  public:
    std::unique_ptr<TerrainObject> terrain;
    std::unique_ptr<Object> entity;
    bool is_selected;
    bool is_on_path;

    Cell(int x, int y, std::unique_ptr<TerrainObject> terrain);
    void draw(ConsoleEngine& engine);

  private:
    int pos_x;
    int pos_y;
};

struct Point {
    int x;
    int y;
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
    Point operator+(const Point& point) const {
        return Point{x + point.x, y + point.y};
    }
};

namespace std {
template <>
struct hash<Point> {
    size_t operator()(const Point& p) const {
        size_t h1 = hash<int>{}(p.x);
        size_t h2 = hash<int>{}(p.y);
        return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
    }
};
}  // namespace std

class Map {
  public:
    Map(int width, int height);
    Cell& get(int x, int y);
    void update();
    int width;
    int height;

  private:
    std::vector<std::vector<Cell>> map;
    ConsoleEngine engine;
    Point cursor_pos;
    Point player_pos;
    std::optional<std::deque<Point>> active_path;
    int walk_iteration;

    void generate();
    void generate_lakes();
    void generate_rivers();
    void generate_rocks();
    void generate_objects();
    void add_gardener();

    void get_player_control();
    void clear_path();
    void draw_path();
    void player_move();

    void redraw(int x, int y);
};

class PathFinder {
  public:
    static std::optional<std::deque<Point>> create_path(Map& map, Point start,
                                                        Point end);

  private:
};

class MyGarden {
  public:
    MyGarden(int width = 100, int height = 20);
    void play();

  private:
    Map map;
};