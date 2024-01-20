#include "entity.h"
#include "raylib.h"
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <sstream>

#define DEBUG

Vector2 inline vecSum(const Vector2 *v1, const Vector2 *v2) {
  return Vector2{v1->x + v2->x, v1->y + v2->y};
}

Vector2 inline vecDiff(const Vector2 *v1, const Vector2 *v2) {
  return Vector2{v1->x - v2->x, v1->y - v2->y};
}

Vector2 inline scalerMul(const Vector2 *vec, float s) {
  return Vector2{vec->x * s, vec->y * s};
}

Vector::Vector(float x, float y) : x(x), y(y) {}
Vector2 Vector::toRaylibVec() { return {this->x, this->y}; }

inline float Vector::lenght() { return std::sqrt(x * x + y * y); }

Vector operator+(const Vector &vec1, const Vector &vec2) {
  return {vec1.x + vec2.x, vec1.y + vec2.y};
}

Vector operator-(const Vector &vec1, const Vector &vec2) {
  return {vec1.x - vec2.x, vec1.y - vec2.y};
}

Vector operator*(const Vector &vec1, const Vector &vec2) {
  return {vec1.x * vec2.x, vec1.y * vec2.y};
}

Vector operator*(const Vector &vec, float scalar) {
  return {vec.x * scalar, vec.y * scalar};
}

// FIXME: remove these
float Vector2Length(const Vector2 &vec) {
  return std::sqrt(vec.x * vec.x + vec.y * vec.y);
}

Vector2 Vector2Normalize(const Vector2 &vec) {
  float length = Vector2Length(vec);
  if (length != 0.0f) {
    return {vec.x / length, vec.y / length};
  }
  return vec;
}
float Vector2DotProduct(const Vector2 &vec1, const Vector2 &vec2) {
  return vec1.x * vec2.x + vec1.y * vec2.y;
}

// --

inline Vector Gravity::getAcceleration() { return {0, Gravity::GRAVITY}; }

#ifdef DEBUG
void drawDebug(Entity *entity) {
  Rectangle bb = entity->getBoundingBox();
  DrawRectangleLinesEx(bb, 0.5, BLACK);
  Vector *velocity = entity->getVelocity();
  std::ostringstream oss;
  oss << "Velocity (" << velocity->x << "," << velocity->y << ")";
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

// ReactionForce::ReactionForce(Entity *target) : target(target){};

// Vector2 ReactionForce::getAcceleration() {
//   Vector2 entityAcc = target->getAccelerationExcept(this);
//   return scalerMul(&entityAcc, -1);
// }

// void ReactionForce::apply(Entity *entity, float time) {
//   const float RESTITUTION_COEF = 0.8f;
//   // stop the object from moving
//   // NOTE: this means reactions must be the first force to be applied, this
//   is
//   // correct since Reaction is a nextFrame force
//   entity->updateVelocity({0, 0});
//   Vector2 acc = getAcceleration();
//   Vector2 displacement = scalerMul(&acc, time * time);
//   entity->updatePosition(vecSum(&displacement, entity->getPosition()));
//   entity->updateVelocity(scalerMul(&acc, time));
// }

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

Entity::~Entity() {};

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

// should the mass be 0 or inf
Bat::Bat(Vector startingPos, Color color, float width, float height)
    : Entity(startingPos, 0), color(color), width(width), height(height) {}
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

// void applyReactionForces(const std::vector<Entity *> *entities) {
//   for (size_t i = 0; i < entities->size(); i++) {
//     for (size_t j = i + 1; j < entities->size(); j++) {
//       Entity *e1 = entities->at(i);
//       Entity *e2 = entities->at(j);
//       if (!isColliding(e1, e2)) {
//         continue;
//       }
//       // FIXME: when we are poping next frame force we must deallocate these
//       ReactionForce *r1 = new ReactionForce(e2);
//       ReactionForce *r2 = new ReactionForce(e1);
//       e2->applyNextFrame(r1);
//       e1->applyNextFrame(r2);
//     }
//   }
// }
