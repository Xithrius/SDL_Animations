#include "SDL3/SDL_rect.h"
#include "entities/entity.h"
#include "isometric_cube.h"

void IsometricCubeEntity::setPosition(const SDL_FPoint& position) {
  current_position = position;

  position0 = {position.x, position.y - wave_dy};
  position1 = {position.x, position.y + wave_dy};

  // Depth-sort by screen y so items higher on screen (smaller y) render first
  setZOrder(current_position.y);
}

SDL_FPoint IsometricCubeEntity::getBehindLeft() {
  SDL_FPoint pos = this->current_position;

  float x = pos.x - size;
  float y = pos.y - size / 2;

  return {x, y};
}

SDL_FPoint IsometricCubeEntity::getBehindRight() {
  SDL_FPoint pos = this->current_position;

  float x = pos.x + size;
  float y = pos.y - size / 2;

  return {x, y};
}

SDL_FPoint IsometricCubeEntity::getFrontLeft() {
  SDL_FPoint pos = this->current_position;

  float x = pos.x - size;
  float y = pos.y + size / 2;

  return {x, y};
}

SDL_FPoint IsometricCubeEntity::getFrontRight() {
  SDL_FPoint pos = this->current_position;

  float x = pos.x + size;
  float y = pos.y + size / 2;

  return {x, y};
}

BoundingBox IsometricCubeEntity::getBoundingBox() const {
  return BoundingBox(this->current_position.x - size,
                     this->current_position.y - size * 2,
                     this->current_position.x + size, this->current_position.y);
}
