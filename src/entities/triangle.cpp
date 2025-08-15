#include "triangle.h"

#include <algorithm>

TriangleEntity::TriangleEntity(const SDL_FPoint& p1, const SDL_FPoint& p2,
                               const SDL_FPoint& p3)
    : point1(p1), point2(p2), point3(p3) {}

void TriangleEntity::update(float) {}

void TriangleEntity::render(SDL_Renderer* renderer) {
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

  if (filled) {
    // Draw filled triangle using SDL_RenderFillTriangle (if available) or
    // manual triangulation For now, just draw the outline
    SDL_RenderLine(renderer, point1.x, point1.y, point2.x, point2.y);
    SDL_RenderLine(renderer, point2.x, point2.y, point3.x, point3.y);
    SDL_RenderLine(renderer, point3.x, point3.y, point1.x, point1.y);
  } else {
    // Draw triangle outline
    SDL_RenderLine(renderer, point1.x, point1.y, point2.x, point2.y);
    SDL_RenderLine(renderer, point2.x, point2.y, point3.x, point3.y);
    SDL_RenderLine(renderer, point3.x, point3.y, point1.x, point1.y);
  }
}

BoundingBox TriangleEntity::getBoundingBox() const {
  float minX = std::min({point1.x, point2.x, point3.x});
  float minY = std::min({point1.y, point2.y, point3.y});
  float maxX = std::max({point1.x, point2.x, point3.x});
  float maxY = std::max({point1.y, point2.y, point3.y});

  return BoundingBox(minX, minY, maxX, maxY);
}

void TriangleEntity::setPoints(const SDL_FPoint& p1, const SDL_FPoint& p2,
                               const SDL_FPoint& p3) {
  point1 = p1;
  point2 = p2;
  point3 = p3;
}
