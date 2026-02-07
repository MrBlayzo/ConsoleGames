#pragma once
#include "ConsoleEngine.h"
#include "RandomGenerator.h"

class Object {
  public:
    Object(char sprite, Color256 color);
    virtual ~Object() = default;
    virtual bool update();
    char get_sprite();
    Color256 get_color();

  protected:
    char sprite;
    Color256 color;

  private:
};

class Gardener : public Object {
  public:
    Gardener();
    bool update() override;

  private:
};

class Ground : public Object {
  public:
    Ground();

  private:
};
class Soil : public Object {
  public:
    Soil();

  private:
};
class Grass : public Object {
  public:
    Grass();

  private:
};
class Path : public Object {
  public:
    Path();

  private:
};
class Water : public Object {
  public:
    Water();

  private:
};
class Rock : public Object {
  public:
    Rock();

  private:
};
class House : public Object {
  public:
    House();

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
    static constexpr int min_growing_time = 50;
    static constexpr int max_growing_time = 100;
    GrowingObject(char sprite, Color256 color, GrowthStatePtr state);

    virtual const GrowthStateFactory& get_factory() const = 0;
    void set_new_state(GrowthStatePtr state);

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