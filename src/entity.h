#ifndef ENTITY_H_
#define ENTITY_H_

#include <stack>
#include <vector>

#include "raylib.h"

class Vector {
public:
  Vector(float x, float y);
  Vector2 toRaylibVec();
  float lenght();
  // arithmetic
  friend Vector operator+(const Vector &vec1, const Vector &vec2);
  friend Vector operator-(const Vector &vec1, const Vector &vec2);
  friend Vector operator*(const Vector &vec1, const Vector &vec2);
  friend Vector operator*(const Vector &vec, float scalar);

  float x;
  float y;
};

class Entity;

class ForceApplicator {
public:
  virtual void apply(Entity *entity, float time) = 0;
  virtual Vector getAcceleration() = 0;
  virtual ~ForceApplicator();
};

class Gravity : public ForceApplicator {
public:
  void apply(Entity *entity, float time) override;
  Vector getAcceleration() override;

private:
  const float GRAVITY = 10;
  Vector *acc;
};

// class ReactionForce : public ForceApplicator {
// public:
//   ReactionForce(Entity *target);
//   Vec2 getAcceleration() override;
//   void apply(Entity *entity, float time) override;

// private:
//   Entity *target;
// };

class Entity {
public:
  Entity(Vector startingPosition, long mass);
  virtual ~Entity();
  void applyContiniously(ForceApplicator *force);
  void applyNextFrame(ForceApplicator *force);
  void update(float time);
  Vector *getPosition();
  Vector *getVelocity();
  // Vec2 getAccelerationExcept(const ForceApplicator *force);
  long getMass();

  void updatePosition(Vector pos);
  void updateVelocity(Vector velocity);

  virtual void draw() = 0; // TODO: why
  virtual Rectangle getBoundingBox() = 0;

private:
  Vector position;
  Vector velocity;
  long mass;
  std::vector<ForceApplicator *> continiousForces;
  std::vector<ForceApplicator *> nextFrameForces;
};

class Ball : public Entity {
public:
  Ball(Vector startingPosition, long mass, Color color, float radius);
  void draw() override;
  Rectangle getBoundingBox() override;

private:
  Color color;
  float radius;
};

class Bat : public Entity {
public:
  Bat(Vector startingPosition, Color color, float width, float height);
  void draw() override;
  Rectangle getBoundingBox() override;

private:
  Color color;
  float width;
  float height;
};

bool isColliding(Entity *e1, Entity *e2);

void applyReactionForces(const std::vector<Entity *> *entities);

#endif // ENTITY_H_
