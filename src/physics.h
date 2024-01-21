#ifndef PHYSICS_H_
#define PHYSICS_H_

#include <sstream>
#include "raylib.h"

class Vector {
public:
  Vector(float x, float y);
  Vector2 toRaylibVec();
  float lenght();
  Vector normalize();
  friend Vector operator+(const Vector &vec1, const Vector &vec2);
  friend Vector operator-(const Vector &vec1, const Vector &vec2);
  friend Vector operator*(const Vector &vec1, const Vector &vec2);
  friend bool operator<(const Vector &vec1, float scalar);
  friend bool operator>(const Vector &vec1, float scalar);
  friend Vector operator*(const Vector &vec, float scalar);
  friend Vector operator/(const Vector &vec, float scalar);
  friend Vector operator/(const Vector &vec1, const Vector &vec2);
  friend std::ostream& operator<<(std::ostream& os, const Vector& vec);

  float x;
  float y;
};

#endif // PHYSICS_H_

