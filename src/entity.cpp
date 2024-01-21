#include "entity.h"
#include "raylib.h"
#include "physics.h"
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <sstream>

#define DEBUG

inline Vector Gravity::getAcceleration() { return {0, Gravity::GRAVITY}; }

#ifdef DEBUG
void drawDebug(Entity *entity) {
  Rectangle bb = entity->getBoundingBox();
  DrawRectangleLinesEx(bb, 0.5, BLACK);
  Vector *velocity = entity->getVelocity();
  std::ostringstream oss;
  oss << "Velocity " << *velocity;
  std::string str = oss.str();
  DrawText(str.c_str(), bb.x + bb.width + 10, bb.y, 10, BLACK);
}
#endif

void Gravity::apply(Entity *entity, float time) {
  Vector gravity = getAcceleration();
  Vector displacement =
      (*entity->getVelocity()) * time + gravity * time * time * 0.5;
  entity->updatePosition(displacement + (*entity->getPosition()));
  entity->updateVelocity((*entity->getVelocity()) + gravity * time);
}

Entity::Entity(Vector startingPos, long mass)
    : position(startingPos), velocity(0, 0), mass(mass) {}

void Entity::applyContiniously(ForceApplicator *force) {
  continiousForces.push_back(force);
}
void Entity::applyNextFrame(ForceApplicator *force) {
  nextFrameForces.push_back(force);
}

void Entity::update(float time) {
  // TODO: figure out if there is a way to use iterators to do this
  while (!nextFrameForces.empty()) {
    ForceApplicator *force = nextFrameForces.back();
    force->apply(this, time);
    nextFrameForces.pop_back();
    delete force;
  }
  for (ForceApplicator *force : continiousForces) {
    force->apply(this, time);
  }
}

Entity::~Entity(){};

ForceApplicator::~ForceApplicator() {}

// Vector2 Entity::getAccelerationExcept(const ForceApplicator *except) {
//   Vector2 acceleration = {0, 0};
//   for (ForceApplicator *force : nextFrameForces) {
//     if (force == except) {
//       continue;
//     }
//     Vector2 fa = force->getAcceleration();
//     acceleration = vecSum(&acceleration, &fa);
//   }
//   for (ForceApplicator *force : continiousForces) {
//     if (force == except) {
//       continue;
//     }
//     Vector2 fa = force->getAcceleration();
//     acceleration = vecSum(&acceleration, &fa);
//   }
//   return acceleration;
// }

long Entity::getMass() { return mass; }

Vector *Entity::getPosition() { return &position; }

Vector *Entity::getVelocity() { return &velocity; }
void Entity::updatePosition(Vector pos) { position = pos; }

void Entity::updateVelocity(Vector vel) { velocity = vel; }

Ball::Ball(Vector startingPos, long mass, Color color, float radius)
    : Entity(startingPos, mass), color(color), radius(radius) {}

Rectangle Ball::getBoundingBox() {
  Vector *center = this->getPosition();
  float radius = this->radius;
  float x = center->x - radius;
  float y = center->y - radius;
  return {x, y, radius * 2, radius * 2};
}

void Ball::draw() {
  Vector *position = this->getPosition();
  DrawCircle(position->x, position->y, this->radius, this->color);
#ifdef DEBUG
  drawDebug(this);
#endif
}

// TODO: make bat weight an argument
Bat::Bat(Vector startingPos, Color color, float width, float height)
    : Entity(startingPos, 1000000), color(color), width(width), height(height) {
}
void Bat::draw() {
  Vector *position = this->getPosition();
  Rectangle rec = {position->x, position->y, this->width, this->height};
  DrawRectangleRec(rec, this->color);
#ifdef DEBUG
  drawDebug(this);
#endif
}

Rectangle Bat::getBoundingBox() {
  // FIXME: this part is common with draw
  Vector *position = this->getPosition();
  return {position->x, position->y, this->width, this->height};
}

inline bool isOverlappingInner(const Rectangle *r1, const Rectangle *r2) {
  float startX = r1->x;
  float endX = startX + r1->width;
  if (!(r2->x > startX && r2->x < endX)) {
    return false;
  }
  float startY = r1->y;
  float endY = startY + r1->height;
  return r2->y > startY && r2->y < endY;
}

inline bool isOverlapping(const Rectangle *r1, const Rectangle *r2) {
  return isOverlappingInner(r1, r2) || isOverlappingInner(r2, r1);
}

bool isColliding(Entity *e1, Entity *e2) {
  Rectangle bb1 = e1->getBoundingBox();
  Rectangle bb2 = e2->getBoundingBox();
  return isOverlapping(&bb1, &bb2);
}

#define RESTITUITION_COEF 0.8f
void resolveCollision(Entity *e1, Entity *e2) {
  Vector v1 = *e1->getVelocity();
  Vector v2 = *e2->getVelocity();
  Vector relativeVelocity = v1 - v2;
  if (relativeVelocity.x > 0 && relativeVelocity.y > 0) {
    return;
  }
  Vector collisionNormal =
      (*e1->getPosition() - *e2->getPosition()).normalize();
  relativeVelocity = relativeVelocity * collisionNormal;
  long m1 = e1->getMass();
  long m2 = e2->getMass();
  float t2 = (1.0f / m1 + 1.0f / m2);

  Vector t1 = (relativeVelocity / t2);
  Vector impluse = t1 * -(1.0f + RESTITUITION_COEF);

  std::ostringstream oss;
  oss << "impluse :" << impluse << " relative velocity: " << relativeVelocity
      << "t1" << t1 << "t2" << t2;
  TraceLog(LOG_INFO, oss.str().c_str());
  e1->updateVelocity(v1 - impluse / (collisionNormal * m1));
  e2->updateVelocity(v2 - impluse / (collisionNormal * m2));
}
