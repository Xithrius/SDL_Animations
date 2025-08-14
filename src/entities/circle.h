#pragma once

#include <SDL3/SDL.h>

#include "entity.h"

class CircleEntity : public Entity {
 private:
  SDL_FPoint center;
  float radius;
  SDL_Color color = {255, 255, 255, 255};
  bool filled = true;
  float borderThickness = 1.0f;

 public:
  CircleEntity(const SDL_FPoint& center, float radius);

  void update(float) override;

  void render(SDL_Renderer* renderer) override;

  // Circle-specific methods
  void setCenter(const SDL_FPoint& center) { this->center = center; }

  void setRadius(float radius) { this->radius = radius; }

  void setColor(const SDL_Color& color) { this->color = color; }

  void setFilled(bool filled) { this->filled = filled; }

  void setBorderThickness(float thickness) { borderThickness = thickness; }

  SDL_FPoint getCenter() const { return center; }

  float getRadius() const { return radius; }
};
