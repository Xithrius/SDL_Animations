#include "line.h"

#include <spdlog/spdlog.h>

LineEntity::LineEntity(const SDL_FPoint& start, const SDL_FPoint& end)
    : start(start), end(end) {}

void LineEntity::update(float) {}

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
