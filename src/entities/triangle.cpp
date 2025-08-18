#include "triangle.h"

#include <algorithm>

TriangleEntity::TriangleEntity(const SDL_FPoint& p1, const SDL_FPoint& p2,
                               const SDL_FPoint& p3)
    : point1(p1), point2(p2), point3(p3) {}

void TriangleEntity::update(float) {}

void TriangleEntity::render(SDL_Renderer* renderer) {
  if (!visible) return;

  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

  if (filled) {
    // Draw filled triangle using lines (simple approach)
    // For a proper filled triangle, you'd need to implement scanline filling
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

void TriangleEntity::setPosition(const SDL_FPoint& position) {
  // Calculate the center of the triangle
  SDL_FPoint center = {(point1.x + point2.x + point3.x) / 3.0f,
                       (point1.y + point2.y + point3.y) / 3.0f};

  // Calculate the offset to move the triangle
  float offsetX = position.x - center.x;
  float offsetY = position.y - center.y;

  // Move all three points by the offset
  point1.x += offsetX;
  point1.y += offsetY;
  point2.x += offsetX;
  point2.y += offsetY;
  point3.x += offsetX;
  point3.y += offsetY;
}

SDL_FPoint TriangleEntity::getPosition() const {
  // Return the center of the triangle
  return SDL_FPoint{(point1.x + point2.x + point3.x) / 3.0f,
                    (point1.y + point2.y + point3.y) / 3.0f};
}

void TriangleEntity::setPoints(const SDL_FPoint& p1, const SDL_FPoint& p2,
                               const SDL_FPoint& p3) {
  point1 = p1;
  point2 = p2;
  point3 = p3;
}
