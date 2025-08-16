#pragma once

#include <SDL3/SDL.h>

#include "entity.h"

class TriangleEntity : public Entity,
                       public IPositionable,
                       public IInteractive {
 private:
  SDL_FPoint point1, point2, point3;
  SDL_Color color = {255, 255, 255, 255};
  bool filled = true;
  bool draggable = true;  // Can be set to false to disable dragging

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

  // Interface implementations
  IPositionable* asPositionable() override { return this; }

  IInteractive* asInteractive() override { return this; }

  // IPositionable implementation
  void setPosition(const SDL_FPoint& position) override;
  SDL_FPoint getPosition() const override;

  // IInteractive implementation
  bool canBeDragged() const override { return draggable; }

  void setDraggable(bool draggable) { this->draggable = draggable; }

  // Triangle-specific methods
  void setPoints(const SDL_FPoint& p1, const SDL_FPoint& p2,
                 const SDL_FPoint& p3);

  void setColor(const SDL_Color& color) { this->color = color; }

  void setFilled(bool filled) { this->filled = filled; }

  SDL_FPoint getPoint1() const { return point1; }

  SDL_FPoint getPoint2() const { return point2; }

  SDL_FPoint getPoint3() const { return point3; }
};
