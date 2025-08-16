#include "line.h"

#include <spdlog/spdlog.h>

#include <cmath>

LineEntity::LineEntity(const SDL_FPoint& start, const SDL_FPoint& end)
    : start(start), end(end) {
  // Calculate the origin (center point) of the line
  origin.x = (start.x + end.x) / 2.0f;
  origin.y = (start.y + end.y) / 2.0f;

  // Calculate the length of the line
  float dx = end.x - start.x;
  float dy = end.y - start.y;
  lineLength = sqrtf(dx * dx + dy * dy);
}

void LineEntity::update(float deltaTime) {
  // Update rotation angle
  angle += rotationSpeed * deltaTime;

  // Keep angle in reasonable range (optional)
  if (angle > 2.0f * M_PI) {
    angle -= 2.0f * M_PI;
  }

  // Calculate new start and end points based on rotation
  float halfLength = lineLength / 2.0f;

  // Calculate the direction vector of the line
  float directionX = cosf(angle);
  float directionY = sinf(angle);

  // Update start and end points
  start.x = origin.x - directionX * halfLength;
  start.y = origin.y - directionY * halfLength;
  end.x = origin.x + directionX * halfLength;
  end.y = origin.y + directionY * halfLength;
}

void LineEntity::render(SDL_Renderer* renderer) {
  if (!visible) return;

  if (!gradientProps.enabled) {
    // Render solid line
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderLine(renderer, start.x, start.y, end.x, end.y);
    return;
  }

  // Render gradient line by drawing multiple line segments
  const int segments = 50;  // Number of segments for gradient
  for (int i = 0; i < segments; ++i) {
    float progress = static_cast<float>(i) / static_cast<float>(segments - 1);

    SDL_Color color;
    color.r = static_cast<Uint8>(
        gradientProps.startColor.r +
        (gradientProps.endColor.r - gradientProps.startColor.r) * progress);
    color.g = static_cast<Uint8>(
        gradientProps.startColor.g +
        (gradientProps.endColor.g - gradientProps.startColor.g) * progress);
    color.b = static_cast<Uint8>(
        gradientProps.startColor.b +
        (gradientProps.endColor.b - gradientProps.startColor.b) * progress);
    color.a = static_cast<Uint8>(
        gradientProps.startColor.a +
        (gradientProps.endColor.a - gradientProps.startColor.a) * progress);

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    // Calculate segment start and end
    SDL_FPoint segStart = {
        start.x + (end.x - start.x) * (static_cast<float>(i) / segments),
        start.y + (end.y - start.y) * (static_cast<float>(i) / segments)};
    SDL_FPoint segEnd = {
        start.x + (end.x - start.x) * (static_cast<float>(i + 1) / segments),
        start.y + (end.y - start.y) * (static_cast<float>(i + 1) / segments)};

    SDL_RenderLine(renderer, segStart.x, segStart.y, segEnd.x, segEnd.y);
  }
}

BoundingBox LineEntity::getBoundingBox() const {
  float halfLength = lineLength / 2.0f;
  float halfThickness = thickness / 2.0f;

  float maxExtent = halfLength + halfThickness;

  return BoundingBox(origin.x - maxExtent, origin.y - maxExtent,
                     origin.x + maxExtent, origin.y + maxExtent);
}

void LineEntity::setPosition(const SDL_FPoint& position) {
  // Calculate the center of the line
  SDL_FPoint center = {(start.x + end.x) / 2.0f, (start.y + end.y) / 2.0f};

  // Calculate the offset to move the line
  float offsetX = position.x - center.x;
  float offsetY = position.y - center.y;

  // Move both start and end points by the offset
  start.x += offsetX;
  start.y += offsetY;
  end.x += offsetX;
  end.y += offsetY;

  // Update origin for rotation if it exists
  if (origin.x != 0.0f || origin.y != 0.0f) {
    origin.x += offsetX;
    origin.y += offsetY;
  }
}

SDL_FPoint LineEntity::getPosition() const {
  // Return the center of the line
  return SDL_FPoint{(start.x + end.x) / 2.0f, (start.y + end.y) / 2.0f};
}
