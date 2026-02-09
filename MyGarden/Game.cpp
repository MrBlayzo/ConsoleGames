#include "Game.h"

#include <algorithm>
#include <array>
#include <deque>
#include <queue>
#include <unordered_map>
#include <unordered_set>

Cell::Cell(int x, int y, std::unique_ptr<TerrainObject> terrain)
    : pos_x(x),
      pos_y(y),
      terrain(std::move(terrain)),
      is_selected(false),
      is_on_path(false) {}

void Cell::draw(ConsoleEngine& engine) {
    engine.set_cursor_to_pos(pos_x, pos_y);
    if (is_selected) engine.set_style(ConsoleStyle::Inverse);
    if (is_on_path) engine.set_background_color(Colors256::Gray80);
    if (entity)
        engine.print_color(entity->get_color(), entity->get_sprite());
    else
        engine.print_color(terrain->get_color(), terrain->get_sprite());
    engine.reset_styles();
}

Map::Map(int width, int height)
    : width(width), height(height), engine(), cursor_pos(0, 0) {
    engine.clear();
    engine.hide_cursor();
    map.resize(height);
    for (int y = 0; y < height; ++y) {
        map[y].reserve(width);
        for (int x = 0; x < width; ++x) {
            map[y].emplace_back(x, y, std::make_unique<Ground>());
        }
    }
    map[cursor_pos.y][cursor_pos.x].is_selected = true;
    generate();
    update();
}

Cell& Map::get(int x, int y) { return map[y][x]; }

void Map::generate() {
    generate_lakes();
    generate_rivers();
    generate_rocks();
    generate_objects();
    add_gardener();
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            redraw(x, y);
        }
    }
}
void Map::generate_lakes() {
    std::unordered_set<Point> seen;
    std::deque<Point> potential;
    constexpr std::array<Point, 4> dirr{{{-1, 0}, {0, -1}, {1, 0}, {0, 1}}};

    int count =
        RandomGenerator::randint(1, std::max(1, (width * height) / 200));
    for (int i = 0; i < count; ++i) {
        int x = RandomGenerator::randint(0, width - 1);
        int y = RandomGenerator::randint(0, height - 1);
        while (seen.contains({x, y})) {
            x = RandomGenerator::randint(0, width - 1);
            y = RandomGenerator::randint(0, height - 1);
        }
        map[y][x].terrain = std::make_unique<Water>();
        potential.emplace_front(x, y);
        seen.emplace(x, y);
    }
    while (potential.size() > 0) {
        auto [x, y] = potential.back();
        potential.pop_back();
        for (auto& d : dirr) {
            int new_x = x + d.x;
            int new_y = y + d.y;
            if (new_x < 0 || new_y < 0 || new_x >= width || new_y >= height)
                continue;
            if (seen.contains({new_x, new_y})) continue;
            seen.emplace(new_x, new_y);
            if (RandomGenerator::rand() < 0.4f) {
                map[new_y][new_x].terrain = std::make_unique<Water>();
                potential.emplace_front(new_x, new_y);
            }
        }
    }
}
void Map::generate_rocks() {
    std::unordered_set<Point> seen;
    std::deque<Point> potential;
    constexpr std::array<Point, 4> dirr{{{-1, 0}, {0, -1}, {1, 0}, {0, 1}}};

    int count =
        RandomGenerator::randint(1, std::max(1, (width * height) / 600));
    for (int i = 0; i < count; ++i) {
        int x = RandomGenerator::randint(0, width - 1);
        int y = RandomGenerator::randint(0, height - 1);
        while (seen.contains({x, y}) ||
               !dynamic_cast<Ground*>(map[y][x].terrain.get())) {
            x = RandomGenerator::randint(0, width - 1);
            y = RandomGenerator::randint(0, height - 1);
        }
        map[y][x].terrain = std::make_unique<Rock>();
        potential.emplace_front(x, y);
        seen.emplace(x, y);
    }
    int dist = 0;
    int curr_size = potential.size();
    while (potential.size() > 0) {
        auto [x, y] = potential.back();
        potential.pop_back();
        for (auto& d : dirr) {
            int new_x = x + d.x;
            int new_y = y + d.y;
            if (new_x < 0 || new_y < 0 || new_x >= width || new_y >= height)
                continue;
            if (seen.contains({new_x, new_y}) ||
                !dynamic_cast<Ground*>(map[new_y][new_x].terrain.get()))
                continue;
            seen.emplace(new_x, new_y);
            if (RandomGenerator::rand() < 0.8f - dist * 0.1f) {
                map[new_y][new_x].terrain = std::make_unique<Rock>();
                potential.emplace_front(new_x, new_y);
            }
        }
        if (--curr_size <= 0) {
            ++dist;
            curr_size = potential.size();
        }
    }
}

void Map::generate_rivers() {
    std::unordered_set<Point> seen;
    constexpr std::array<Point, 4> dirr{{{-1, 0}, {0, -1}, {1, 0}, {0, 1}}};

    int count =
        RandomGenerator::randint(1, std::max(1, (width * height) / 400));
    for (int i = 0; i < count; ++i) {
        int x = RandomGenerator::randint(0, width - 1);
        int y = RandomGenerator::randint(0, height - 1);
        while (seen.contains({x, y})) {
            x = RandomGenerator::randint(0, width - 1);
            y = RandomGenerator::randint(0, height - 1);
        }
        map[y][x].terrain = std::make_unique<Water>();
        seen.emplace(x, y);

        auto [dx, dy] = dirr[RandomGenerator::randint(0, 3)];
        while (true) {
            if (RandomGenerator::rand() < 0.1f) {
                Point next_dir;
                do {
                    next_dir = dirr[RandomGenerator::randint(0, 3)];
                } while (next_dir.x == dx && next_dir.y == dy);
                dx = next_dir.x;
                dy = next_dir.y;
                x = x + dx;
                y = y + dy;
            } else {
                if (dx == 0) {
                    y = y + dy;
                    x = x + dx + RandomGenerator::randint(-1, 1);
                } else {
                    x = x + dx;
                    y = y + dy + RandomGenerator::randint(-1, 1);
                    ;
                }
            }

            if (x < 0 || y < 0 || x >= width || y >= height) break;
            if (seen.contains({x, y})) break;
            seen.emplace(x, y);
            map[y][x].terrain = std::make_unique<Water>();
        }
    }
}

void Map::generate_objects() {
    enum class ObjectTypes {
        Ground,
        Grass,
        Water,
        Rock,
        Tree,
    };
    auto count_neighbors = [&](const std::vector<std::vector<ObjectTypes>>& map,
                               int x, int y, ObjectTypes type, int radius = 1) {
        int count = 0;
        for (int dy = -radius; dy <= radius; ++dy) {
            for (int dx = -radius; dx <= radius; ++dx) {
                if (dx == 0 && dy == 0) continue;
                int nx = x + dx, ny = y + dy;
                if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                    if (map[ny][nx] == type) count++;
                }
            }
        }
        return count;
    };

    std::vector<std::vector<ObjectTypes>> generated(
        height, std::vector<ObjectTypes>(width, ObjectTypes::Ground));
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (dynamic_cast<Water*>(map[y][x].terrain.get())) {
                generated[y][x] = ObjectTypes::Water;
            } else if (dynamic_cast<Rock*>(map[y][x].terrain.get())) {
                generated[y][x] = ObjectTypes::Rock;
            }
        }
    }

    const int iterations = 2;
    for (int iter = 0; iter < iterations; ++iter) {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                if (generated[y][x] != ObjectTypes::Ground) continue;

                float r = RandomGenerator::rand();
                bool has_grass_near =
                    count_neighbors(generated, x, y, ObjectTypes::Grass, 1) > 0;
                float grass_prob = has_grass_near ? 0.2f : 0.02f;

                if (r < grass_prob) {
                    generated[y][x] = ObjectTypes::Grass;
                    continue;
                }

                r = RandomGenerator::rand();
                bool has_tree_near_3 =
                    count_neighbors(generated, x, y, ObjectTypes::Tree, 2) > 0;
                bool has_tree_near_1 =
                    count_neighbors(generated, x, y, ObjectTypes::Tree, 1) > 0;
                float tree_prob =
                    has_tree_near_1 ? 0 : (has_tree_near_3 ? 0.1f : 0.03f);

                if (r < tree_prob) {
                    generated[y][x] = ObjectTypes::Tree;
                    continue;
                }
            }
        }
    }

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            switch (generated[y][x]) {
                case ObjectTypes::Grass:
                    map[y][x].terrain = std::make_unique<Grass>();
                    break;
                case ObjectTypes::Tree:
                    map[y][x].entity = std::make_unique<Tree>();
                    break;
                default:
                    break;
            }
        }
    }
}
void Map::add_gardener() {
    int x = RandomGenerator::randint(0, width - 1);
    int y = RandomGenerator::randint(0, height - 1);
    while (!(dynamic_cast<Ground*>(map[y][x].terrain.get()) ||
             dynamic_cast<Grass*>(map[y][x].terrain.get()))) {
        x = RandomGenerator::randint(0, width - 1);
        y = RandomGenerator::randint(0, height - 1);
    }
    map[y][x].entity = std::make_unique<Gardener>();
    player_pos = Point(x, y);
}
void Map::redraw(int x, int y) { map[y][x].draw(engine); }
void Map::update() {
    get_player_control();
    player_move();
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (map[y][x].entity && map[y][x].entity->update()) {
                redraw(x, y);
            }
        }
    }
    engine.set_cursor_to_pos(cursor_pos.x, cursor_pos.y);
}

void Map::clear_path() {
    if (!active_path.has_value()) return;
    for (auto& point : active_path.value()) {
        map[point.y][point.x].is_on_path = false;
        redraw(point.x, point.y);
    }
}
void Map::draw_path() {
    if (!active_path.has_value()) return;
    for (auto& point : active_path.value()) {
        map[point.y][point.x].is_on_path = true;
        redraw(point.x, point.y);
    }
}

void Map::get_player_control() {
    auto old_cursor_pos = cursor_pos;
    auto c = engine.get_no_wait();
    while (c != '\0') {
        if (c == 'a') {
            cursor_pos.x = std::max(0, cursor_pos.x - 1);
        } else if (c == 'd') {
            cursor_pos.x = std::min(width - 1, cursor_pos.x + 1);
        } else if (c == 'w') {
            cursor_pos.y = std::max(0, cursor_pos.y - 1);
        } else if (c == 's') {
            cursor_pos.y = std::min(height - 1, cursor_pos.y + 1);
        } else if (c == '\n' || c == 'f') {
            clear_path();
            walk_iteration = 0;
            active_path = PathFinder::create_path(*this, player_pos, cursor_pos);
            draw_path();
        }
        c = engine.get_no_wait();
    }
    if (old_cursor_pos != cursor_pos) {
        map[old_cursor_pos.y][old_cursor_pos.x].is_selected = false;
        map[cursor_pos.y][cursor_pos.x].is_selected = true;
        redraw(old_cursor_pos.x, old_cursor_pos.y);
        redraw(cursor_pos.x, cursor_pos.y);
    }
}

std::optional<std::deque<Point>> PathFinder::create_path(Map& map, Point start,
                                                      Point end) {
    struct PathPoint {
        Point pos;
        Point parent_pos;
        double cost;
        double target_cost;
        bool calculated = false;
    };
    constexpr std::array<Point, 4> dirr{{{-1, 0}, {0, -1}, {1, 0}, {0, 1}}};

    std::unordered_map<Point, PathPoint> points;
    auto cmp = [&points](const Point left, const Point right) {
        return points[left].cost + points[left].target_cost >
               points[right].cost + points[right].target_cost;
    };
    std::priority_queue<Point, std::vector<Point>, decltype(cmp)> pending(cmp);

    points[start] = PathPoint(
        start, start, 0, std::abs(end.x - start.x) + std::abs(end.y - start.y));
    pending.push(start);

    while (!pending.empty()) {
        auto point_pos = pending.top();
        pending.pop();
        auto& point = points[point_pos];
        if (point.calculated) continue;
        point.calculated = true;
        for (auto& d : dirr) {
            Point new_pos{point_pos + d};
            if (new_pos.x < 0 || new_pos.x >= map.width || new_pos.y < 0 ||
                new_pos.y >= map.height) {
                continue;
            }
            if (map.get(new_pos.x, new_pos.y).terrain->get_passability() <= 0) {
                continue;
            }
            if (map.get(new_pos.x, new_pos.y).entity) {
                continue;
            }
            if (points.contains(new_pos) && points[new_pos].calculated) {
                continue;
            }

            double new_cost =
                point.cost +
                map.get(new_pos.x, new_pos.y).terrain->get_passability();

            if (!points.contains(new_pos) || new_cost < points[new_pos].cost) {
                points[new_pos] = PathPoint(
                    new_pos, point_pos, new_cost,
                    std::abs(end.x - new_pos.x) + std::abs(end.y - new_pos.y));

                if (new_pos == end) {
                    std::deque<Point> path;
                    auto current = end;
                    while (current != start) {
                        path.push_front(current);
                        current = points[current].parent_pos;
                    }
                    path.push_front(start);
                    return path;
                }
                pending.push(new_pos);
            }
        }
    }

    return std::nullopt;
}



void Map::player_move() {
    if (!active_path.has_value()) return;
    if (active_path.value().empty()) return;
    auto next_point = active_path.value().front();
    if (++walk_iteration <
        map[next_point.y][next_point.x].terrain->get_passability())
        return;

    walk_iteration = 0;
    active_path.value().pop_front();

    map[player_pos.y][player_pos.x].entity.reset();
    if (!dynamic_cast<Water*>(map[player_pos.y][player_pos.x].terrain.get())) {
        map[player_pos.y][player_pos.x].terrain = std::make_unique<Path>();
    }
    map[player_pos.y][player_pos.x].is_on_path = false;
    redraw(player_pos.x, player_pos.y);

    map[next_point.y][next_point.x].entity = std::make_unique<Gardener>();
    redraw(next_point.x, next_point.y);
    player_pos = next_point;
}

MyGarden::MyGarden(int width, int height) : map(width, height) {}
void MyGarden::play() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        map.update();
    }
}