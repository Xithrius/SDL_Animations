#pragma once

#include <SDL3/SDL.h>

#include <vector>

#include "entity.h"

class LineEntity : public Entity {
 public:
  // Gradient properties
  struct GradientProperties {
    bool enabled = false;
    SDL_Color startColor = {255, 255, 255, 255};
    SDL_Color endColor = {0, 0, 255, 255};
    std::vector<SDL_Color> stops;  // Custom gradient stops
  };

 private:
  SDL_FPoint start;
  SDL_FPoint end;
  float thickness = 1.0f;
  SDL_Color color = {255, 255, 255, 255};

  GradientProperties gradientProps;

 public:
  LineEntity(const SDL_FPoint& start, const SDL_FPoint& end);

  void render(SDL_Renderer* renderer) override;

  // Line-specific methods
  void setStart(const SDL_FPoint& start) { this->start = start; }

  void setEnd(const SDL_FPoint& end) { this->end = end; }

  void setThickness(float thickness) { this->thickness = thickness; }

  void setColor(const SDL_Color& color) { this->color = color; }

  void setGradientProperties(const GradientProperties& props) {
    gradientProps = props;
  }

  SDL_FPoint getStart() const { return start; }

  SDL_FPoint getEnd() const { return end; }
};
