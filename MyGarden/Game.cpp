#include "Game.h"

#include <algorithm>

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
    : width(width), height(height), engine(), player(*this) {
    engine.clear();
    engine.hide_cursor();
    map.resize(height);
    for (int y = 0; y < height; ++y) {
        map[y].reserve(width);
        for (int x = 0; x < width; ++x) {
            map[y].emplace_back(x, y, std::make_unique<Ground>());
        }
    }
    map[player.cursor_pos.y][player.cursor_pos.x].is_selected = true;
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
    redraw_all();
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
    player.pos = Point(x, y);
}
void Map::redraw(int x, int y) { map[y][x].draw(engine); }
void Map::redraw_all() {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            redraw(x, y);
        }
    }
}
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
}

void Map::clear_path() {
    if (!player.active_path.has_value()) return;
    for (auto& point : player.active_path.value()) {
        map[point.y][point.x].is_on_path = false;
        redraw(point.x, point.y);
    }
}
void Map::draw_path() {
    if (!player.active_path.has_value()) return;
    for (auto& point : player.active_path.value()) {
        map[point.y][point.x].is_on_path = true;
        redraw(point.x, point.y);
    }
}

void Map::get_player_control() {
    auto old_cursor_pos = player.cursor_pos;
    auto c = engine.get_no_wait();
    while (c != '\0') {
        if (c == 'a') {
            player.cursor_pos.x = std::max(0, player.cursor_pos.x - 1);
        } else if (c == 'd') {
            player.cursor_pos.x = std::min(width - 1, player.cursor_pos.x + 1);
        } else if (c == 'w') {
            player.cursor_pos.y = std::max(0, player.cursor_pos.y - 1);
        } else if (c == 's') {
            player.cursor_pos.y = std::min(height - 1, player.cursor_pos.y + 1);
        } else if (c == 'f') {
            player.create_path();
        } else if (c == '\r') {
            player.new_action();
        }
        c = engine.get_no_wait();
    }
    if (old_cursor_pos != player.cursor_pos) {
        map[old_cursor_pos.y][old_cursor_pos.x].is_selected = false;
        map[player.cursor_pos.y][player.cursor_pos.x].is_selected = true;
        redraw(old_cursor_pos.x, old_cursor_pos.y);
        redraw(player.cursor_pos.x, player.cursor_pos.y);
    }
}

double Map::get_passability(int x, int y) {
    if (map[y][x].entity) return -1.0;
    return map[y][x].terrain->get_passability();
}
double Map::get_passability(Point p) {
    if (map[p.y][p.x].entity) return -1.0;
    return map[p.y][p.x].terrain->get_passability();
}

void Map::set_new_terrain(int x, int y,
                          std::unique_ptr<TerrainObject> terrain) {
    map[y][x].terrain = std::move(terrain);
    redraw(x, y);
}
void Map::set_new_entity(int x, int y, std::unique_ptr<Object> entity) {
    map[y][x].entity = std::move(entity);
    redraw(x, y);
}
void Map::reset_entity(int x, int y) {
    map[y][x].entity.reset();
    redraw(x, y);
}

void Map::player_move() {
    Point old_pos = player.pos;
    if (!player.update()) return;

    map[old_pos.y][old_pos.x].entity.reset();
    if (!dynamic_cast<Water*>(map[old_pos.y][old_pos.x].terrain.get()) &&
        !dynamic_cast<Bridge*>(map[old_pos.y][old_pos.x].terrain.get())) {
        map[old_pos.y][old_pos.x].terrain = std::make_unique<Path>();
    }
    map[old_pos.y][old_pos.x].is_on_path = false;
    redraw(old_pos.x, old_pos.y);

    map[player.pos.y][player.pos.x].entity = std::make_unique<Gardener>();
    redraw(player.pos.x, player.pos.y);
}

std::vector<PlayerActionTypes> Map::get_available_action(int x, int y){
    if(map[y][x].entity && !dynamic_cast<Gardener*>(map[y][x].entity.get())){
        return map[y][x].entity->get_available_actions();
    }
    return map[y][x].terrain->get_available_actions();
}

std::vector<Buildings> Map::get_available_buildings(int x, int y){
    return map[y][x].terrain->get_available_buildings();
}

int Menu::show_options_menu(ConsoleEngine& engine, int width, int heigth,
                            int pos_x, int pos_y,
                            std::vector<MenuOption> options) {
    return Menu(engine, width, heigth, pos_x, pos_y, options).get_option();
}
Menu::Menu(ConsoleEngine& engine, int width, int height, int pos_x, int pos_y,
           std::vector<MenuOption> options)
    : engine(engine),
      width(width),
      pos_x(pos_x),
      pos_y(pos_y),
      height(height),
      options(options),
      current_option(0) {}

void Menu::set_relative_pos(int x, int y) {
    engine.set_cursor_to_pos(pos_x + x, pos_y + y);
}

void Menu::draw() {
    for (int x = 0; x < width; ++x) {
        set_relative_pos(x, 0);
        engine.print('#');
    }
    for (int x = 0; x < width; ++x) {
        set_relative_pos(x, height - 1);
        engine.print('#');
    }
    for (int y = 0; y < height; ++y) {
        set_relative_pos(0, y);
        engine.print('#');
    }
    for (int y = 0; y < height; ++y) {
        set_relative_pos(width - 1, y);
        engine.print('#');
    }
    for (int y = 1; y < height - 1; ++y) {
        set_relative_pos(1, y);
        engine.print(std::string(width - 2, ' '));
    }
    for (int option = 0; option < options.size(); ++option) {
        set_relative_pos((width - 2 - options[option].param.size()) / 2, option + 1);
        if (option == current_option)
            engine.set_background_color(Colors256::Gray50);
        engine.print(options[option].param);
        if (option == current_option) engine.reset_styles();
    }
}
int Menu::get_option() {
    draw();
    char c;
    do {
        c = engine.get_no_wait();
        if (c == 'w') {
            select_option(current_option - 1);
        } else if (c == 's') {
            select_option(current_option + 1);
        } else if (c == '\r') {
            return current_option;
        } else if (c == 27) {
            return -1;
        }
    } while (true);
    return 0;
}
void Menu::select_option(int option) {
    option = std::max(0, std::min((int)options.size() - 1, option));
    if (option == current_option) return;
    set_relative_pos((width - 2 - options[current_option].param.size()) / 2,
                     current_option + 1);
    engine.print(options[current_option].param);

    current_option = option;

    engine.set_background_color(Colors256::Gray50);
    set_relative_pos((width - 2 - options[current_option].param.size()) / 2,
                     current_option + 1);
    engine.print(options[current_option].param);
    engine.reset_styles();
}

MyGarden::MyGarden(int width, int height) : map(width, height) {}
void MyGarden::play() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        map.update();
    }
}