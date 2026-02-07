#include "Game.h"

#include <array>
#include <deque>
#include <set>
#include <thread>

Cell::Cell(int x, int y, std::unique_ptr<Object> content)
    : pos_x(x), pos_y(y), content(std::move(content)), is_selected(false) {}

void Cell::draw(ConsoleEngine& engine) {
    engine.set_cursor_to_pos(pos_x, pos_y);
    if (is_selected) engine.set_style(ConsoleStyle::Inverse);
    engine.print_color(content->get_color(), content->get_sprite());
    engine.reset_styles();
}

Map::Map(int width, int height)
    : width(width), height(height), engine(), cursor_x(0), cursor_y(0) {
    engine.clear();
    engine.hide_cursor();
    map.resize(height);
    for (int row = 0; row < height; ++row) {
        map[row].reserve(width);
        for (int col = 0; col < width; ++col) {
            map[row].emplace_back(col, row, std::make_unique<Ground>());
            if (row == cursor_y && col == cursor_x) {
                map[row].back().is_selected = true;
            }
        }
    }
    generate();
    update();
}
void Map::generate() {
    generate_lakes();
    generate_rivers();
    generate_rocks();
    generate_objects();
    for (int row = 0; row < height; ++row) {
        for (int col = 0; col < width; ++col) {
            map[row][col].draw(engine);
        }
    }
}
void Map::generate_lakes() {
    using Pos = std::pair<int, int>;
    std::set<Pos> seen;
    std::deque<Pos> potential;
    constexpr std::array<Pos, 4> dirr{{{-1, 0}, {0, -1}, {1, 0}, {0, 1}}};

    int count =
        RandomGenerator::randint(1, std::max(1, (width * height) / 200));
    for (int i = 0; i < count; ++i) {
        int x = RandomGenerator::randint(0, width - 1);
        int y = RandomGenerator::randint(0, height - 1);
        while (seen.contains({x, y})) {
            x = RandomGenerator::randint(0, width - 1);
            y = RandomGenerator::randint(0, height - 1);
        }
        map[y][x].content = std::make_unique<Water>();
        potential.emplace_front(x, y);
        seen.emplace(x, y);
    }
    while (potential.size() > 0) {
        auto [x, y] = potential.back();
        potential.pop_back();
        for (auto& [dx, dy] : dirr) {
            int new_x = x + dx;
            int new_y = y + dy;
            if (new_x < 0 || new_y < 0 || new_x >= width || new_y >= height)
                continue;
            if (seen.contains({new_x, new_y})) continue;
            seen.emplace(new_x, new_y);
            if (RandomGenerator::rand() < 0.4f) {
                map[new_y][new_x].content = std::make_unique<Water>();
                potential.emplace_front(new_x, new_y);
            }
        }
    }
}
void Map::generate_rocks() {
    using Pos = std::pair<int, int>;
    std::set<Pos> seen;
    std::deque<Pos> potential;
    constexpr std::array<Pos, 4> dirr{{{-1, 0}, {0, -1}, {1, 0}, {0, 1}}};

    int count =
        RandomGenerator::randint(1, std::max(1, (width * height) / 600));
    for (int i = 0; i < count; ++i) {
        int x = RandomGenerator::randint(0, width - 1);
        int y = RandomGenerator::randint(0, height - 1);
        while (seen.contains({x, y}) ||
               !dynamic_cast<Ground*>(map[y][x].content.get())) {
            x = RandomGenerator::randint(0, width - 1);
            y = RandomGenerator::randint(0, height - 1);
        }
        map[y][x].content = std::make_unique<Rock>();
        potential.emplace_front(x, y);
        seen.emplace(x, y);
    }
    int dist = 0;
    int curr_size = potential.size();
    while (potential.size() > 0) {
        auto [x, y] = potential.back();
        potential.pop_back();
        for (auto& [dx, dy] : dirr) {
            int new_x = x + dx;
            int new_y = y + dy;
            if (new_x < 0 || new_y < 0 || new_x >= width || new_y >= height)
                continue;
            if (seen.contains({new_x, new_y}) ||
                !dynamic_cast<Ground*>(map[new_y][new_x].content.get()))
                continue;
            seen.emplace(new_x, new_y);
            if (RandomGenerator::rand() < 0.8f-dist*0.1f) {
                map[new_y][new_x].content = std::make_unique<Rock>();
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
    using Pos = std::pair<int, int>;
    std::set<Pos> seen;
    constexpr std::array<Pos, 4> dirr{{{-1, 0}, {0, -1}, {1, 0}, {0, 1}}};

    int count =
        RandomGenerator::randint(1, std::max(1, (width * height) / 400));
    for (int i = 0; i < count; ++i) {
        int x = RandomGenerator::randint(0, width - 1);
        int y = RandomGenerator::randint(0, height - 1);
        while (seen.contains({x, y})) {
            x = RandomGenerator::randint(0, width - 1);
            y = RandomGenerator::randint(0, height - 1);
        }
        map[y][x].content = std::make_unique<Water>();
        seen.emplace(x, y);

        auto [dx, dy] = dirr[RandomGenerator::randint(0, 3)];
        while (true) {
            if (RandomGenerator::rand() < 0.1f) {
                auto [new_dx, new_dy] = dirr[RandomGenerator::randint(0, 3)];
                while (new_dx == dx && new_dy == dy) {
                    std::tie(new_dx, new_dy) =
                        dirr[RandomGenerator::randint(0, 3)];
                }
                dx = new_dx;
                dy = new_dy;
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
            map[y][x].content = std::make_unique<Water>();
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
    for (int row = 0; row < height; ++row) {
        for (int col = 0; col < width; ++col) {
            if (dynamic_cast<Water*>(map[row][col].content.get())) {
                generated[row][col] = ObjectTypes::Water;
            }
            else if (dynamic_cast<Rock*>(map[row][col].content.get())) {
                generated[row][col] = ObjectTypes::Rock;
            }
        }
    }

    const int iterations = 2;
    for (int iter = 0; iter < iterations; ++iter) {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                if (generated[y][x] != ObjectTypes::Ground)
                    continue;  // пропускаем фиксированные

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
                    map[y][x].content = std::make_unique<Grass>();
                    break;
                case ObjectTypes::Tree:
                    map[y][x].content = std::make_unique<Tree>();
                    break;
                default:
                    break;
            }
        }
    }
}
void Map::update() {
    for (int row = 0; row < height; ++row) {
        for (int col = 0; col < width; ++col) {
            if (map[row][col].content->update()) {
                map[row][col].draw(engine);
            }
        }
    }
    engine.set_cursor_to_pos(cursor_x, cursor_y);
}
MyGarden::MyGarden(int width, int height) : map(width, height) {}
void MyGarden::play() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        map.update();
    }
}