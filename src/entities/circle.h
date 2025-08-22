#pragma once

#include <SDL3/SDL.h>

#include "entity.h"

class CircleEntity : public Entity, public IPositionable, public IInteractive {
 private:
  SDL_FPoint center;
  float radius;
  SDL_Color color = {255, 255, 255, 255};
  bool filled = true;
  float borderThickness = 1.0f;
  bool draggable = true;  // Can be set to false to disable dragging

 public:
  CircleEntity(const SDL_FPoint& center, float radius);

  void update(float deltaTime) override;

  void render(SDL_Renderer* renderer) override;

  // Entity type identification
  EntityType getEntityType() const override {
    static EntityType typeId =
        EntityTypeRegistry::getInstance().registerType("Circle");
    return typeId;
  }

  // Debug methods
  BoundingBox getBoundingBox() const override;

  // Interface implementations
  IPositionable* asPositionable() override { return this; }

  IInteractive* asInteractive() override { return this; }

  // IPositionable implementation
  void setPosition(const SDL_FPoint& position) override { setCenter(position); }

  SDL_FPoint getPosition() const override { return center; }

  // IInteractive implementation
  bool canBeDragged() const override { return draggable; }

  void setDraggable(bool draggable) { this->draggable = draggable; }

  // Circle-specific methods
  void setCenter(const SDL_FPoint& center) { this->center = center; }

  void setRadius(float radius) { this->radius = radius; }

  void setColor(const SDL_Color& color) { this->color = color; }

  void setFilled(bool filled) { this->filled = filled; }

  void setBorderThickness(float thickness) { borderThickness = thickness; }

  SDL_FPoint getCenter() const { return center; }

  float getRadius() const { return radius; }
};
