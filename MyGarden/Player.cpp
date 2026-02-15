#include "Player.h"

#include "Game.h"
#include "PathFinder.h"

PlayerAction::PlayerAction(Map& map, Point pos)
    : map(map), execute_iteration(0), is_executed(false), pos(pos) {}
DigAction::DigAction(Map& map, Point pos) : PlayerAction(map, pos) {}
PlaceAction::PlaceAction(Map& map, Point pos,
                         std::unique_ptr<GrowingObject> new_object)
    : PlayerAction(map, pos), new_object(std::move(new_object)) {}
BuildAction::BuildAction(Map& map, Point pos,
                         std::unique_ptr<TerrainObject> new_object)
    : PlayerAction(map, pos), new_object(std::move(new_object)) {}

void PlayerAction::execute() {
    if (is_executed) return;
    if (++execute_iteration < get_execution_time()) return;
    finish();
    is_executed = true;
}
bool PlayerAction::executed() { return is_executed; }

void DigAction::finish() {
    map.reset_entity(pos.x, pos.y);
    map.redraw(pos.x, pos.y);
}
void PlaceAction::finish() {
    map.set_new_entity(pos.x, pos.y, std::move(new_object));
    map.redraw(pos.x, pos.y);
}
void BuildAction::finish() {
    map.set_new_terrain(pos.x, pos.y, std::move(new_object));
    map.redraw(pos.x, pos.y);
}

Player::Player(Map& map) : map(map), cursor_pos(0, 0) {}
bool Player::update() {
    if (active_path.has_value() && !active_path.value().empty()) {
        auto next_point = active_path.value().front();
        if (++walk_iteration < map.get_passability(next_point)) return false;

        walk_iteration = 0;
        active_path.value().pop_front();
        pos = next_point;
        return true;
    }
    if (active_action) {
        active_action->execute();
        if (active_action->executed()) {
            active_action.reset();
            return true;
        }
    }

    return false;
}

void Player::create_path() {
    map.clear_path();
    walk_iteration = 0;

    if (map.get(cursor_pos.x, cursor_pos.y).entity)
        active_path = PathFinder::create_path_to_area(map, pos, cursor_pos);
    else
        active_path = PathFinder::create_path_to_point(map, pos, cursor_pos);

    map.draw_path();
}

void Player::create_path_to_area() {
    map.clear_path();
    walk_iteration = 0;

    active_path = PathFinder::create_path_to_area(map, pos, cursor_pos);

    map.draw_path();
}

void Player::new_action() {
    std::vector<PlayerActionTypes> actions =
        map.get_available_action(cursor_pos.x, cursor_pos.y);

    std::vector<MenuOption> menu_options;
    for (auto& action : actions) {
        menu_options.emplace_back(action_to_string(action), action);
    }

    int chose =
        Menu::show_options_menu(map.engine, 20, 10, (map.width - 20) / 2,
                                (map.height - 10) / 2, menu_options);
    map.redraw_all();

    if (chose < 0) return;  // TODO: исправить на optional
    PlayerActionTypes chosed =
        std::any_cast<PlayerActionTypes>(menu_options[chose].return_param);
    if (chosed == PlayerActionTypes::Move) {
        create_path();
    } else if (chosed == PlayerActionTypes::Place) {
        int chose_object = Menu::show_options_menu(
            map.engine, 20, 10, (map.width - 20) / 2, (map.height - 10) / 2,
            {{"Flower", 0},
             {"Tree", 1}});
        map.redraw_all();
        if (chose_object < 0) return;
        create_path_to_area();
        if (chose_object == 0)
            active_action = std::make_unique<PlaceAction>(
                map, cursor_pos, std::make_unique<Flower>());
        else if (chose_object == 1)
            active_action = std::make_unique<PlaceAction>(
                map, cursor_pos, std::make_unique<Tree>());
    } else if (chosed == PlayerActionTypes::Build) {
        std::vector<Buildings> buildings =
            map.get_available_buildings(cursor_pos.x, cursor_pos.y);

        std::vector<MenuOption> menu_buildings_options;
        for (auto& building : buildings) {
            menu_buildings_options.emplace_back(building_type_to_string(building), building);
        }

        int chose_object_ind = Menu::show_options_menu(
            map.engine, 20, 10, (map.width - 20) / 2, (map.height - 10) / 2,
            menu_buildings_options);
        map.redraw_all();
        if (chose_object_ind < 0) return;
        Buildings chose_object =
            std::any_cast<Buildings>(menu_buildings_options[chose_object_ind].return_param);
        create_path_to_area();
        if (chose_object == Buildings::Bridge)
            active_action = std::make_unique<BuildAction>(
                map, cursor_pos, std::make_unique<Bridge>());
        else if (chose_object == Buildings::House)
            active_action = std::make_unique<BuildAction>(
                map, cursor_pos, std::make_unique<House>());
    } else {
        create_path_to_area();
        active_action = std::make_unique<DigAction>(map, cursor_pos);
    }
}
