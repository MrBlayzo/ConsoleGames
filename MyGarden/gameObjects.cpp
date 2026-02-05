#include "GameObjects.h"

Object::Object(char sprite, Color256 color) : sprite(sprite), color(color) {}

char Object::get_sprite() { return sprite; }
Color256 Object::get_color() { return color; }

GrowingObject::GrowingObject(char sprite, Color256 color, GrowingState state)
    : Object(sprite, color), state(state), grow_iteration(0) {}

Gardener::Gardener() : Object('@', Colors256::Yellow) {}
Ground::Ground() : Object('.', Colors256::GrayBrown) {}
Soil::Soil() : Object('#', Colors256::LightBrown) {}
Grass::Grass() : Object('"', Colors256::DarkGreen) {}
Path::Path() : Object('_', Colors256::White) {}
Water::Water() : Object('~', Colors256::Blue) {}
House::House() : Object('H', Colors256::OrangeBrown) {}
Vegetable::Vegetable(GrowingState state)
    : GrowingObject('c', Colors256::Red, state) {}
Flower::Flower(GrowingState state)
    : GrowingObject('f', Colors256::Purple, state) {}
Tree::Tree(GrowingState state) : GrowingObject('i', Color256(28), state) {}

bool Gardener::update() { return false; }
bool Ground::update() { return false; }
bool Soil::update() { return false; }
bool Grass::update() { return false; }
bool Path::update() { return false; }
bool Water::update() { return false; }
bool House::update() { return false; }

bool Vegetable::update() {
    ++grow_iteration;
    if (state == GrowingState::Planted && grow_iteration > 50) {
        set_new_state(GrowingState::Growing);
        return true;
    } else if (state == GrowingState::Growing && grow_iteration > 50) {
        set_new_state(GrowingState::Ready);
        return true;
    }
    return false;
}
void Vegetable::set_new_state(GrowingState new_state) {
    state = new_state;
    grow_iteration = 0;
    if (state == GrowingState::Growing) {
        sprite = 'c';
    } else if (state == GrowingState::Ready) {
        sprite = 'C';
    }
}
bool Flower::update() {
    ++grow_iteration;
    if (state == GrowingState::Planted && grow_iteration > 50) {
        set_new_state(GrowingState::Growing);
        return true;
    } else if (state == GrowingState::Growing && grow_iteration > 50) {
        set_new_state(GrowingState::Ready);
        return true;
    }
    return false;
}
void Flower::set_new_state(GrowingState new_state) {
    state = new_state;
    grow_iteration = 0;
    if (state == GrowingState::Growing) {
        sprite = 'f';
    } else if (state == GrowingState::Ready) {
        sprite = 'F';
    }
}
bool Tree::update() {
    ++grow_iteration;
    if (state == GrowingState::Planted && grow_iteration > 50) {
        set_new_state(GrowingState::Growing);
        return true;
    } else if (state == GrowingState::Growing && grow_iteration > 50) {
        set_new_state(GrowingState::Ready);
        return true;
    }
    return false;
}
void Tree::set_new_state(GrowingState new_state) {
    state = new_state;
    grow_iteration = 0;
    if (state == GrowingState::Growing) {
        sprite = 't';
    } else if (state == GrowingState::Ready) {
        sprite = 'T';
    }
}