#pragma once

#include <SDL3/SDL.h>

#include "entity.h"

class RectangleEntity : public Entity {
 private:
  SDL_FRect rect;
  SDL_Color color = {255, 255, 255, 255};
  bool filled = true;
  float borderThickness = 1.0f;

 public:
  RectangleEntity(const SDL_FRect& rect);

  void update(float deltaTime) override;

  void render(SDL_Renderer* renderer) override;

  // Entity type identification
  EntityType getEntityType() const override {
    static EntityType typeId = EntityTypeRegistry::registerType("Rectangle");
    return typeId;
  }

  // Rectangle-specific methods
  void setRect(const SDL_FRect& rect) { this->rect = rect; }

  void setColor(const SDL_Color& color) { this->color = color; }

  void setFilled(bool filled) { this->filled = filled; }

  void setBorderThickness(float thickness) { borderThickness = thickness; }

  SDL_FRect getRect() const { return rect; }
};
