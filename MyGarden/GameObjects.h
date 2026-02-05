#pragma once
#include "ConsoleEngine.h"

class Object {
  public:
    Object(char sprite, Color256 color);
    virtual ~Object() = default;
    virtual bool update() = 0;
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
    bool update() override;

  private:
};
class Soil : public Object {
  public:
    Soil();
    bool update() override;

  private:
};
class Grass : public Object {
  public:
    Grass();
    bool update() override;

  private:
};
class Path : public Object {
  public:
    Path();
    bool update() override;

  private:
};
class Water : public Object {
  public:
    Water();
    bool update() override;

  private:
};
class House : public Object {
  public:
    House();
    bool update() override;

  private:
};

enum class GrowingState { Planted, Growing, Ready };

class GrowingObject : public Object {
  public:
    GrowingObject(char sprite, Color256 color,
                  GrowingState state = GrowingState::Planted);

  protected:
    GrowingState state;
    int grow_iteration;

    virtual void set_new_state(GrowingState state) = 0;
};

class Vegetable : public GrowingObject {
  public:
    Vegetable(GrowingState state = GrowingState::Planted);
    bool update() override;

  private:
    void set_new_state(GrowingState state) override;
};
class Flower : public GrowingObject {
  public:
    Flower(GrowingState state = GrowingState::Planted);
    bool update() override;

  private:
    void set_new_state(GrowingState state) override;
};
class Tree : public GrowingObject {
  public:
    Tree(GrowingState state = GrowingState::Planted);
    bool update() override;

  private:
    void set_new_state(GrowingState state) override;
};