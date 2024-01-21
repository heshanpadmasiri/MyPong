#include "entity.h"
#include "physics.h"
#include "raylib.h"
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
  std::ostringstream oss;
  oss << "Velocity " << *entity->getVelocity() << "\n Position "
      << *entity->getPosition();
  std::string str = oss.str();
  DrawText(str.c_str(), bb.x + bb.width + 10, bb.y, 10, BLACK);
}
#endif

void Gravity::apply(Entity *entity, float time) {
  Vector gravity = getAcceleration();
  entity->updateVelocity((*entity->getVelocity()) + gravity * time);
}

Entity::Entity(long id, Vector startingPos, long mass)
    : id(id), position(startingPos), velocity(0, 0), mass(mass) {}

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
  this->updatePosition(*this->getPosition() + (*this->getVelocity() * time));
}

Entity::~Entity(){};

ForceApplicator::~ForceApplicator() {}

long Entity::getMass() { return mass; }

Vector *Entity::getPosition() { return &position; }

Vector *Entity::getVelocity() { return &velocity; }
void Entity::updatePosition(Vector pos) { position = pos; }

void Entity::updateVelocity(Vector vel) { velocity = vel; }

Ball::Ball(Vector startingPos, long mass, Color color, float radius)
    : Entity(0, startingPos, mass), color(color), radius(radius) {}

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

Bat::Bat(Vector startingPos, long mass, Color color, float width, float height)
    : Entity(1, startingPos, mass), color(color), width(width), height(height) {
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
#define WEIGHT_SCALE 0.01f
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
  float m1 = e1->getMass() * WEIGHT_SCALE;
  float m2 = e2->getMass() * WEIGHT_SCALE;
  Vector impluse = (relativeVelocity / ((1.0f / m1) + (1.0f / m2))) *
                   -(1.0f + RESTITUITION_COEF);
  e1->updateVelocity(v1 + impluse / (collisionNormal * m1));
  e2->updateVelocity(v2 - impluse / (collisionNormal * m2));
#ifdef DEBUG
  std::ostringstream oss;
  oss << "impluse :" << impluse << " relative velocity: " << relativeVelocity
      << "v1: " << *e1->getVelocity() << " v2 :" << *e2->getVelocity();
  TraceLog(LOG_INFO, oss.str().c_str());
#endif
}
