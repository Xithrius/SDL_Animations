#include <cmath>

#include "isometric_cube.h"

void IsometricCubeEntity::update(float deltaTime) {
  time += deltaTime * movementSpeed;

  float t = (sin(time - M_PI / 2.0f) + 1.0f) / 2.0f;

  // Interpolate between position0 and position1
  current_position.x = position0.x + t * (position1.x - position0.x);
  current_position.y = position0.y + t * (position1.y - position0.y);
}
