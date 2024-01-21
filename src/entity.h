#ifndef ENTITY_H_
#define ENTITY_H_

#include <stack>
#include <vector>

#include "raylib.h"
#include "physics.h"

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
};

class Entity {
public:
  Entity(long id, Vector startingPosition, long mass);
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
  long id;
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
  Bat(Vector startingPosition, long mass, Color color, float width, float height);
  void draw() override;
  Rectangle getBoundingBox() override;

private:
  Color color;
  float width;
  float height;
};

bool isColliding(Entity *e1, Entity *e2);

void resolveCollision(Entity *e1, Entity *e2) ;
#endif // ENTITY_H_
