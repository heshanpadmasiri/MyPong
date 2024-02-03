#include "physics.h"

#include <cmath>
#include <sstream>

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

Vector operator/(const Vector &vec, float scalar) {
  return {vec.x / scalar, vec.y / scalar};
}

Vector operator/(const Vector &vec1, const Vector &vec2) {
  return { vec1.x / vec2.x, vec1.y / vec2.y };
}

std::ostream& operator<<(std::ostream& os, const Vector& vec) {
    os << "(" << vec.x << ", " << vec.y << ")";
    return os;
}

Vector Vector::normalize() {
  float len = lenght();
  if (len == 0.0f) {
    return {x, y};
  }
  return {x / len, y / len};
}
