#include "circle.h"

#include <spdlog/spdlog.h>

#include <cmath>

CircleEntity::CircleEntity(const SDL_FPoint& center, float radius)
    : center(center), radius(radius) {}

void CircleEntity::update(float) {}

void CircleEntity::render(SDL_Renderer* renderer) {
  if (!visible) return;

  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

  // Draw circle using multiple line segments
  const int segments = 32;
  const float angleStep = 2.0f * M_PI / segments;

  std::vector<SDL_FPoint> points;
  points.reserve(segments + 1);

  for (int i = 0; i <= segments; ++i) {
    float angle = i * angleStep;
    SDL_FPoint point = {center.x + radius * cosf(angle),
                        center.y + radius * sinf(angle)};
    points.push_back(point);
  }

  if (filled) {
    // Draw filled circle by connecting points to center
    for (size_t i = 0; i < points.size() - 1; ++i) {
      SDL_RenderLine(renderer, center.x, center.y, points[i].x, points[i].y);
    }
  } else {
    // Draw circle outline
    for (size_t i = 0; i < points.size() - 1; ++i) {
      SDL_RenderLine(renderer, points[i].x, points[i].y, points[i + 1].x,
                     points[i + 1].y);
    }
  }
}
