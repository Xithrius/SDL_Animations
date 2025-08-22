#pragma once

#include <SDL3/SDL.h>

#include "entity.h"

class RectangleEntity : public Entity,
                        public IPositionable,
                        public IInteractive {
 private:
  SDL_FRect rect;
  SDL_Color color = {255, 255, 255, 255};
  bool filled = true;
  float borderThickness = 1.0f;
  bool draggable = true;  // Can be set to false to disable dragging

 public:
  RectangleEntity(const SDL_FRect& rect);

  void update(float deltaTime) override;

  void render(SDL_Renderer* renderer) override;

  // Entity type identification
  EntityType getEntityType() const override {
    static EntityType typeId =
        EntityTypeRegistry::getInstance().registerType("Rectangle");
    return typeId;
  }

  // Debug methods
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

  // Rectangle-specific methods
  void setRect(const SDL_FRect& rect) { this->rect = rect; }

  void setColor(const SDL_Color& color) { this->color = color; }

  void setFilled(bool filled) { this->filled = filled; }

  void setBorderThickness(float thickness) { borderThickness = thickness; }

  SDL_FRect getRect() const { return rect; }
};
