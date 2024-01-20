#ifndef ENTITY_H_
#define ENTITY_H_

#include <stack>
#include <vector>

#include "raylib.h"

class Vec2 {
public:
  Vec2(float x, float y);
  Vector2 toRaylibVec();

private:
  float x;
  float y;
};

class Entity;

class ForceApplicator {
public:
  virtual void apply(Entity *entity, float time) = 0;
  virtual Vector2 getAcceleration() = 0;
  virtual ~ForceApplicator();
};

class Gravity : public ForceApplicator {
public:
  void apply(Entity *entity, float time) override;
  Vector2 getAcceleration() override;

private:
  const float GRAVITY = 10;
};

class ReactionForce : public ForceApplicator {
public:
  ReactionForce(Entity *target);
  Vector2 getAcceleration() override;
  void apply(Entity *entity, float time) override;

private:
  Entity *target;
};

class Entity {
public:
  Entity(Vector2 startingPosition, long mass);
  void applyContiniously(ForceApplicator *force);
  void applyNextFrame(ForceApplicator *force);
  void update(float time);
  Vector2 *getPosition();
  Vector2 *getVelocity();
  Vector2 getAccelerationExcept(const ForceApplicator *force);
  long getMass();

  void updatePosition(Vector2 pos);
  void updateVelocity(Vector2 velocity);

  virtual void draw() = 0; // TODO: why
  virtual Rectangle getBoundingBox() = 0;

private:
  Vector2 position;
  Vector2 velocity;
  long mass;
  std::vector<ForceApplicator *> continiousForces;
  std::vector<ForceApplicator *> nextFrameForces;
};

class Ball : public Entity {
public:
  Ball(Vector2 startingPosition, long mass, Color color, float radius);
  void draw() override;
  Rectangle getBoundingBox() override;

private:
  Color color;
  float radius;
};

class Bat : public Entity {
public:
  Bat(Vector2 startingPosition, Color color, float width, float height);
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
