#pragma once
#include <array>
#include <deque>
#include <memory>
#include <optional>
#include <queue>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <functional>

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

struct Player{
    Point pos;
    std::optional<std::deque<Point>> active_path;
    int walk_iteration;
};

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
    Player player;
    
    void generate();
    void generate_lakes();
    void generate_rivers();
    void generate_rocks();
    void generate_objects();
    void add_gardener();

    void clear_path();
    void draw_path();

    void get_player_control();
    void player_move();

    void redraw(int x, int y);
    void redraw_all();
};

class Menu{
  public:
    static int show_options_menu(ConsoleEngine& engine, int width, int heigth, int pos_x, int pos_y, std::vector<std::string> options);
  private:
    Menu(ConsoleEngine& engine, int width, int height, int pos_x, int pos_y, std::vector<std::string> options);
    void draw();
    int get_option();
    void select_option(int option);
    void set_relative_pos(int x, int y);
    ConsoleEngine& engine;
    int width;
    int height;
    int pos_x;
    int pos_y;
    std::vector<std::string> options;
    int current_option;
};

class PathFinder {
  public:
    static std::optional<std::deque<Point>> create_path_to_point(Map& map, Point start,
                                                        Point end);
    static std::optional<std::deque<Point>> create_path_to_area(Map& map, Point start,
                                                        Point end);

  private:
    struct PathPoint {
        Point pos;
        Point parent_pos;
        double cost;
        double target_cost;
        bool calculated = false;
    };
    static constexpr std::array<Point, 4> dirr{
        {{-1, 0}, {0, -1}, {1, 0}, {0, 1}}};
    struct ComparePoints {
        std::unordered_map<Point, PathPoint>& points_map;
        ComparePoints(std::unordered_map<Point, PathPoint>& points_map)
            : points_map(points_map) {};
        bool operator()(const Point& a, const Point& b) const {
            return points_map[a].cost + points_map[a].target_cost >
                   points_map[b].cost + points_map[b].target_cost;
        }
    };

    PathFinder(Map& map, Point start, Point end, std::function<bool(Point)> is_target);
    std::optional<std::deque<Point>> find_path();
    void explore_point(Point current_pos);
    void look_at_new(Point new_pos, Point current_pos, double current_cost);
    std::deque<Point> convert_path();
    double dist_to_target(const Point& p) const;

    Map& map_;
    const Point start_;
    const Point end_;
    std::unordered_map<Point, PathPoint> points_;
    ComparePoints comparator_;
    std::priority_queue<Point, std::vector<Point>, ComparePoints> pending_;
    std::optional<Point> found_target_;
    std::function<bool(Point)> is_target_;
};

class MyGarden {
  public:
    MyGarden(int width = 100, int height = 20);
    void play();

  private:
    Map map;
};