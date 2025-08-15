#pragma once

#include <SDL3/SDL.h>
#include <string>

#include "entities/entity.h"

class TextEntity : public Entity {
 private:
  std::string text;
  int x, y;
  SDL_Color color;

 public:
  TextEntity(const std::string& text, int x, int y, 
             SDL_Color color = {255, 255, 255, 255});

  void update(float deltaTime) override;
  void render(SDL_Renderer* renderer) override;
  BoundingBox getBoundingBox() const override;

  // Text-specific methods
  void setText(const std::string& text) { this->text = text; }
  void setPosition(int x, int y) { this->x = x; this->y = y; }
  void setColor(SDL_Color color) { this->color = color; }
  
  const std::string& getText() const { return text; }
  int getX() const { return x; }
  int getY() const { return y; }
  SDL_Color getColor() const { return color; }
};
