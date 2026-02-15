#pragma once
#include <memory>
#include <vector>

#include "ConsoleEngine.h"
#include "RandomGenerator.h"

namespace PassabilityCoefs {
inline constexpr double ground = 2;
inline constexpr double soil = 4;
inline constexpr double grass = 8;
inline constexpr double path = 1;
inline constexpr double water = 16;
inline constexpr double rock = -1;
inline constexpr double bridge = 1;

};  // namespace PassabilityCoefs

enum class PlayerActionTypes;
enum class Buildings{
  House, 
  Bridge
};
inline std::string building_type_to_string(Buildings building){
    switch (building)
    {
    case Buildings::House:
        return "House";
    case Buildings::Bridge:
        return "Bridge";
    default:
        return "";
    }
}

class Object {
  public:
    Object(char sprite, Color256 color);
    virtual ~Object() = default;
    virtual bool update();
    char get_sprite();
    Color256 get_color();
    virtual constexpr bool passable(){return false;};
    virtual std::vector<PlayerActionTypes> get_available_actions();

  protected:
    char sprite;
    Color256 color;

  private:
};

class TerrainObject : public Object {
  public:
    TerrainObject(char sprite, Color256 color);
    virtual constexpr double get_passability() { return -1.0; };
    virtual std::vector<Buildings> get_available_buildings();
    constexpr bool passable(){return true;};
};

class Gardener : public Object {
  public:
    Gardener();
    bool update() override;

  private:
};

class Ground : public TerrainObject {
  public:
    static constexpr double passability = PassabilityCoefs::ground;
    Ground();

    constexpr double get_passability() override { return passability; };
    std::vector<PlayerActionTypes> get_available_actions() override;
    std::vector<Buildings> get_available_buildings() override;

  private:
};
class Soil : public TerrainObject {
  public:
    static constexpr double passability = PassabilityCoefs::soil;
    Soil();

    constexpr double get_passability() override { return passability; };
    std::vector<PlayerActionTypes> get_available_actions() override;
    std::vector<Buildings> get_available_buildings() override;

  private:
};
// TODO: исправить - трава, здания - это не TerrainObject
class Grass : public TerrainObject {
  public:
    static constexpr double passability = PassabilityCoefs::grass;
    Grass();

    constexpr double get_passability() override { return passability; };
    std::vector<PlayerActionTypes> get_available_actions() override;
    std::vector<Buildings> get_available_buildings() override;

  private:
};
class Path : public TerrainObject {
  public:
    static constexpr double passability = PassabilityCoefs::path;
    Path();

    constexpr double get_passability() override { return passability; };
    std::vector<PlayerActionTypes> get_available_actions() override;
    std::vector<Buildings> get_available_buildings() override;

  private:
};
class Bridge : public TerrainObject {
  public:
    static constexpr double passability = PassabilityCoefs::bridge;
    Bridge();

    constexpr double get_passability() override { return passability; };
    std::vector<PlayerActionTypes> get_available_actions() override;

  private:
};
class Water : public TerrainObject {
  public:
    static constexpr double passability = PassabilityCoefs::water;
    Water();

    constexpr double get_passability() override { return passability; };
    std::vector<PlayerActionTypes> get_available_actions() override;
    std::vector<Buildings> get_available_buildings() override;

  private:
};
class Rock : public TerrainObject {
  public:
    static constexpr double passability = PassabilityCoefs::rock;
    Rock();

    constexpr double get_passability() override { return passability; };
    std::vector<PlayerActionTypes> get_available_actions() override;

  private:
};
class House : public TerrainObject {
  public:
    static constexpr double passability = PassabilityCoefs::rock;
    House();

    constexpr double get_passability() override { return passability; };
    std::vector<PlayerActionTypes> get_available_actions() override;


  private:
};

class GrowingObject;

struct GrowthState {
  public:
    GrowthState(int min_growing_time, int max_growing_time, char sprite);
    virtual ~GrowthState() = default;
    virtual bool update(GrowingObject& obj) = 0;
    int get_growing_time();
    char get_sprite();

  protected:
    const int min_growing_time;
    const int max_growing_time;
    const char sprite;
    const int growing_time;
};

class PlantedState : public GrowthState {
  public:
    PlantedState(int min_growing_time, int max_growing_time, char sprite);
    bool update(GrowingObject& obj) override;
};
class GrowingState : public GrowthState {
  public:
    GrowingState(int min_growing_time, int max_growing_time, char sprite);
    bool update(GrowingObject& obj) override;
};
class ReadyState : public GrowthState {
  public:
    ReadyState(int min_growing_time, int max_growing_time, char sprite);
    bool update(GrowingObject& obj) override;
};

using GrowthStatePtr = std::unique_ptr<GrowthState>;

class GrowthStateFactory {
  public:
    virtual ~GrowthStateFactory() = default;
    virtual GrowthStatePtr create_planted() const = 0;
    virtual GrowthStatePtr create_growing() const = 0;
    virtual GrowthStatePtr create_ready() const = 0;
};

class VegetableStateFactory : public GrowthStateFactory {
  public:
    GrowthStatePtr create_planted() const override;
    GrowthStatePtr create_growing() const override;
    GrowthStatePtr create_ready() const override;
};

class FlowerStateFactory : public GrowthStateFactory {
  public:
    GrowthStatePtr create_planted() const override;
    GrowthStatePtr create_growing() const override;
    GrowthStatePtr create_ready() const override;
};

class TreeStateFactory : public GrowthStateFactory {
  public:
    GrowthStatePtr create_planted() const override;
    GrowthStatePtr create_growing() const override;
    GrowthStatePtr create_ready() const override;
};

class GrowingObject : public Object {
  public:
    GrowingObject(char sprite, Color256 color, GrowthStatePtr state);

    virtual const GrowthStateFactory& get_factory() const = 0;
    void set_new_state(GrowthStatePtr state);
    std::vector<PlayerActionTypes> get_available_actions() override;

    int grow_iteration;

  protected:
    GrowthStatePtr state;
};

class Vegetable : public GrowingObject {
  public:
    static const VegetableStateFactory state_factory;
    Vegetable();
    Vegetable(GrowthStatePtr state);
    bool update() override;
    const GrowthStateFactory& get_factory() const override;

  private:
};
class Flower : public GrowingObject {
  public:
    static const FlowerStateFactory state_factory;
    Flower();
    Flower(GrowthStatePtr state);
    bool update() override;
    const GrowthStateFactory& get_factory() const override;

  private:
};
class Tree : public GrowingObject {
  public:
    static const TreeStateFactory state_factory;
    Tree();
    Tree(GrowthStatePtr state);
    bool update() override;
    const GrowthStateFactory& get_factory() const override;

  private:
};