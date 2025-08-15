#pragma once

#include <SDL3/SDL.h>

#include "entity.h"

class TriangleEntity : public Entity {
 private:
  SDL_FPoint point1, point2, point3;
  SDL_Color color = {255, 255, 255, 255};
  bool filled = true;

 public:
  TriangleEntity(const SDL_FPoint& p1, const SDL_FPoint& p2,
                 const SDL_FPoint& p3);

  void update(float deltaTime) override;
  void render(SDL_Renderer* renderer) override;

  // Entity type identification - automatically registers "Triangle"
  EntityType getEntityType() const override {
    static EntityType typeId = EntityTypeRegistry::registerType("Triangle");
    return typeId;
  }

  BoundingBox getBoundingBox() const override;

  // Triangle-specific methods
  void setPoints(const SDL_FPoint& p1, const SDL_FPoint& p2,
                 const SDL_FPoint& p3);

  void setColor(const SDL_Color& color) { this->color = color; }

  void setFilled(bool filled) { this->filled = filled; }

  SDL_FPoint getPoint1() const { return point1; }

  SDL_FPoint getPoint2() const { return point2; }

  SDL_FPoint getPoint3() const { return point3; }
};
