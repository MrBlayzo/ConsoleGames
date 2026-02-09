#include "GameObjects.h"

Object::Object(char sprite, Color256 color) : sprite(sprite), color(color) {}

char Object::get_sprite() { return sprite; }
Color256 Object::get_color() { return color; }
TerrainObject::TerrainObject(char sprite, Color256 color)
    : Object(sprite, color) {}

GrowingObject::GrowingObject(char sprite, Color256 color, GrowthStatePtr state)
    : Object(sprite, color), state(std::move(state)), grow_iteration(0) {}

GrowthState::GrowthState(int min_growing_time, int max_growing_time,
                         char sprite)
    : min_growing_time(min_growing_time),
      max_growing_time(max_growing_time),
      sprite(sprite),
      growing_time(
          RandomGenerator::randint(min_growing_time, max_growing_time)) {}
int GrowthState::get_growing_time() { return growing_time; }
char GrowthState::get_sprite() { return sprite; }

PlantedState::PlantedState(int min_growing_time, int max_growing_time,
                           char sprite)
    : GrowthState(min_growing_time, max_growing_time, sprite) {}
GrowingState::GrowingState(int min_growing_time, int max_growing_time,
                           char sprite)
    : GrowthState(min_growing_time, max_growing_time, sprite) {}
ReadyState::ReadyState(int min_growing_time, int max_growing_time, char sprite)
    : GrowthState(min_growing_time, max_growing_time, sprite) {}

bool PlantedState::update(GrowingObject& obj) {
    if (++obj.grow_iteration >= growing_time) {
        obj.set_new_state(obj.get_factory().create_growing());
        return true;
    }
    return false;
}
bool GrowingState::update(GrowingObject& obj) {
    if (++obj.grow_iteration >= growing_time) {
        obj.set_new_state(obj.get_factory().create_ready());
        return true;
    }
    return false;
}
bool ReadyState::update(GrowingObject& obj) { return false; }

GrowthStatePtr VegetableStateFactory::create_planted() const {
    return std::make_unique<PlantedState>(50, 100, 'c');
}
GrowthStatePtr VegetableStateFactory::create_growing() const {
    return std::make_unique<GrowingState>(50, 100, 'c');
}
GrowthStatePtr VegetableStateFactory::create_ready() const {
    return std::make_unique<ReadyState>(50, 100, 'C');
}

GrowthStatePtr FlowerStateFactory::create_planted() const {
    return std::make_unique<PlantedState>(50, 100, 'f');
}
GrowthStatePtr FlowerStateFactory::create_growing() const {
    return std::make_unique<GrowingState>(50, 100, 'f');
}
GrowthStatePtr FlowerStateFactory::create_ready() const {
    return std::make_unique<ReadyState>(50, 100, 'F');
}

GrowthStatePtr TreeStateFactory::create_planted() const {
    return std::make_unique<PlantedState>(50, 300, 'i');
}
GrowthStatePtr TreeStateFactory::create_growing() const {
    return std::make_unique<GrowingState>(1000, 5000, 't');
}
GrowthStatePtr TreeStateFactory::create_ready() const {
    return std::make_unique<ReadyState>(50, 100, 'T');
}

Gardener::Gardener() : Object('@', Colors256::Yellow) {}
Ground::Ground() : TerrainObject('.', Colors256::GrayBrown) {}
Soil::Soil() : TerrainObject('#', Colors256::LightBrown) {}
Grass::Grass() : TerrainObject('"', Colors256::DarkGreen) {}
Path::Path() : TerrainObject(':', Color256(130)) {}
Water::Water() : TerrainObject('~', Colors256::Blue) {}
Rock::Rock() : TerrainObject('^', Color256(242)) {}
House::House() : Object('H', Colors256::OrangeBrown) {}

Vegetable::Vegetable()
    : GrowingObject('c', Colors256::Red, get_factory().create_planted()) {}
Vegetable::Vegetable(GrowthStatePtr state)
    : GrowingObject('c', Colors256::Red, std::move(state)) {}
Flower::Flower()
    : GrowingObject('f', Colors256::Purple, get_factory().create_planted()) {}
Flower::Flower(GrowthStatePtr state)
    : GrowingObject('f', Colors256::Purple, std::move(state)) {}
Tree::Tree()
    : GrowingObject('i', Color256(28), get_factory().create_planted()) {}
Tree::Tree(GrowthStatePtr state)
    : GrowingObject('i', Color256(28), std::move(state)) {}

bool Object::update() { return false; }
bool Gardener::update() { return false; }

void GrowingObject::set_new_state(GrowthStatePtr new_state) {
    state = std::move(new_state);
    grow_iteration = 0;
    sprite = state->get_sprite();
}

bool Vegetable::update() { return state->update(*this); }
bool Flower::update() { return state->update(*this); }
bool Tree::update() { return state->update(*this); }

const VegetableStateFactory Vegetable::state_factory{};
const FlowerStateFactory Flower::state_factory{};
const TreeStateFactory Tree::state_factory{};

const GrowthStateFactory& Vegetable::get_factory() const {
    return state_factory;
}
const GrowthStateFactory& Flower::get_factory() const { return state_factory; }
const GrowthStateFactory& Tree::get_factory() const { return state_factory; }