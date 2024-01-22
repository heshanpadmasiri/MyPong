#ifndef ENTITY_H_
#define ENTITY_H_

#include <stack>
#include <vector>

#include "physics.h"
#include "raylib.h"

class Entity;

class ForceApplicator {
public:
  void apply(Entity *entity, float time);
  virtual Vector getAcceleration() = 0;
  virtual ~ForceApplicator();
};

class Gravity : public ForceApplicator {
public:
  Vector getAcceleration() override;

private:
  const float GRAVITY = 10;
};

enum Direction { UP, DOWN, LEFT, RIGHT };

// represent force applied by pressign a key
class MotionForce : public ForceApplicator {
public:
  MotionForce(float pressureScale, Direction direction);
  Vector getAcceleration() override;

private:
  float pressureScale;
  Direction direction;
};

class Entity {
public:
  Entity(long id, Vector startingPosition, long mass);
  virtual ~Entity();
  void applyContiniously(ForceApplicator *force);
  void applyNextFrame(ForceApplicator *force);
  void addCollidingEntity(Entity *other);
  void update(float time);
  Vector *getPosition();
  Vector *getVelocity();
  long getMass();
  bool couldSkipCollisionCheck();

  void updatePosition(Vector pos);
  void updateVelocity(Vector velocity);
  long getId();

  virtual void draw() = 0;
  virtual Rectangle getBoundingBox() = 0;

protected:
  bool immovable;
  // NOTE: If both objects has this flag we will skip collision check
  bool skipCollisionCheck;

private:
  long id;
  Vector position;
  Vector velocity;
  long mass;
  std::vector<ForceApplicator *> continiousForces;
  std::vector<ForceApplicator *> nextFrameForces;
  std::stack<Entity *> collidingEntities;
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
  Bat(Vector startingPosition, long mass, Color color, float width,
      float height);
  void draw() override;
  Rectangle getBoundingBox() override;

private:
  Color color;
  float width;
  float height;
};

class FramePart : public Entity {
public:
  FramePart(Rectangle rectange, long mass, Color color,
            long id); // NOTE: they need mass so impact model works
  void draw() override;
  Rectangle getBoundingBox() override;

private:
  Color color;
  Rectangle rectangle;
};

bool isColliding(Entity *e1, Entity *e2);

void resolveCollision(Entity *e1, Entity *e2);
#endif // ENTITY_H_
