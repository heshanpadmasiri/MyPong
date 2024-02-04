#include "entity.h"
#include "physics.h"
#include "raylib.h"
#include <cmath>
#include <cstdlib>
#include <sstream>
#include <vector>

#define DEBUG

#ifdef DEBUG
void drawDebug(Entity *entity) {
  Rectangle bb = entity->getBoundingBox();
  DrawRectangleLinesEx(bb, 0.5, BLACK);
  std::ostringstream oss;
  oss << "Velocity " << *entity->getVelocity() << "\n Position "
      << *entity->getPosition();
  std::string str = oss.str();
  DrawText(str.c_str(), bb.x + bb.width + 10, bb.y, 10, RED);
}
#endif

ForceApplicator::~ForceApplicator() {}
void ForceApplicator::apply(Entity *entity, float time) {
  Vector gravity = getAcceleration();
  entity->updateVelocity((*entity->getVelocity()) + gravity * time);
}

Vector Gravity::getAcceleration() { return {0, Gravity::GRAVITY}; }

MotionForce::MotionForce(float pressureScale, Direction direction)
    : pressureScale(pressureScale), direction(direction) {}

Vector MotionForce::getAcceleration() {
  Vector base = {0, 0};
  switch (direction) {
  case UP:
    base = {0, -1};
    break;
  case DOWN:
    base = {0, 1};
    break;
  case LEFT:
    base = {-1, 0};
    break;
  case RIGHT:
    base = {1, 0};
    break;
  }
  return base * pressureScale;
}

Entity::Entity(long id, Vector startingPos, long mass)
    : immovable(false), skipCollisionCheck(false), id(id),
      position(startingPos), velocity(0, 0), mass(mass) {}

void Entity::applyContiniously(ForceApplicator *force) {
  continiousForces.push_back(force);
}
void Entity::applyNextFrame(ForceApplicator *force) {
  nextFrameForces.push_back(force);
}

static inline float clipIfInRange(float newVal, float oldVal, float start,
                                  float end) {
  if (newVal == oldVal) {
    return newVal;
  }
  if (newVal > start && newVal < end) {
    // TODO: think about a better way to do this
    if (abs(oldVal - end) < abs(oldVal - start)) {
      return end + 0.000001f;
    }
    return start - 0.000001f;
    // return oldVal > end ? end + CLIP_GAP : start - CLIP_GAP;
  }
  return newVal;
}

static Vector clipPositionChange(Vector newPosition, Vector oldPosition,
                                 Rectangle boundingBox) {
  float x = clipIfInRange(newPosition.x, oldPosition.x, boundingBox.x,
                          boundingBox.x + boundingBox.width);
  float y = clipIfInRange(newPosition.y, oldPosition.y, boundingBox.y,
                          boundingBox.y + boundingBox.height);
  return {x, y};
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
  Vector currentPosition = *this->getPosition();
  Vector newPosition = currentPosition + (*this->getVelocity() * time);
  // TODO: this needs to get checked if and only if positions are updating
  while (!collidingEntities.empty()) {
    Entity *other = collidingEntities.top();
    collidingEntities.pop();
#ifdef DEBUG
    // std::ostringstream oss;
    // Rectangle bb = other->getBoundingBox();
    // oss << "clipping overlapping boxes for " << id << " due to collision with "
    //     << other->id << " target" << newPosition << "rectange corner" << bb.x
    //     << "," << bb.y;
#endif
    newPosition = clipPositionChange(newPosition, currentPosition,
                                     other->getBoundingBox());
#ifdef DEBUG
    // oss << " result " << newPosition;
    // TraceLog(LOG_INFO, oss.str().c_str());
#endif
  }
  this->updatePosition(newPosition);
}

bool Entity::couldSkipCollisionCheck() { return skipCollisionCheck; }

Entity::~Entity() {
  while (!nextFrameForces.empty()) {
    ForceApplicator *force = nextFrameForces.back();
    nextFrameForces.pop_back();
    delete force;
  }
  for (ForceApplicator *force : continiousForces) {
    delete force;
  }
  continiousForces.clear();
};

long Entity::getMass() { return mass; }

Vector *Entity::getPosition() { return &position; }

Vector *Entity::getVelocity() { return &velocity; }
void Entity::updatePosition(Vector pos) {
  if (immovable) {
    return;
  }
  position = pos;
}

void Entity::updateVelocity(Vector vel) {
  if (immovable) {
    return;
  }
  velocity = vel;
}

void Entity::addCollidingEntity(Entity *other) {
  collidingEntities.push(other);
}

long Entity::getId() { return id; }

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

inline Vector rectangeCenter(const Rectangle *r) {
  return {r->x + (r->width / 2), r->y + (r->height / 2)};
}

FramePart::FramePart(Rectangle rectangle, long mass, Color color, long id)
    : Entity(id, rectangeCenter(&rectangle), mass), color(color),
      rectangle(rectangle) {
  immovable = true;
  skipCollisionCheck = true;
}

void FramePart::draw() { DrawRectangleRec(rectangle, color); }
Rectangle FramePart::getBoundingBox() { return rectangle; }

static inline bool isInRange(float val, float start, float end) {
  return val >= start && val <= end;
}

// FIXME:
static inline bool isOverlappingInner(const Rectangle *r1,
                                      const Rectangle *r2) {
  float startX = r1->x;
  float endX = startX + r1->width;
  float startY = r1->y;
  float endY = startY + r1->height;
  return (isInRange(startX, r2->x, r2->x + r2->width) ||
          isInRange(endX, r2->x, r2->x + r2->width)) &&
         (isInRange(startY, r2->y, r2->y + r2->height) ||
          isInRange(endY, r2->y, r2->y + r2->height));
}

static inline bool isOverlapping(const Rectangle *r1, const Rectangle *r2) {
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
  e1->addCollidingEntity(e2);
  e2->addCollidingEntity(e1);
// #ifdef DEBUG
//   std::ostringstream oss;
//   oss << "resolving collision for" << e1->getId() << " ," << e2->getId()
//       << " impluse :" << impluse << " relative velocity: " << relativeVelocity
//       << "v1: " << *e1->getVelocity() << " v2 :" << *e2->getVelocity();
//   TraceLog(LOG_INFO, oss.str().c_str());
// #endif
}
