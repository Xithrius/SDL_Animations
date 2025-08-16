#include "point.h"

#include <cmath>

#include "core/app_state.h"

PointEntity::PointEntity(AppState* appState, size_t trailLength, float speed)
    : appState(appState), trailLength(trailLength), speed(speed) {
  trail.resize(trailLength);
  // Initialize trail with current position
  for (size_t i = 0; i < trailLength; ++i) {
    trail[i] = {0.0f, 0.0f};
  }
}

void PointEntity::update(float deltaTime) {
  // Update point movement based on speed
  if (!trail.empty() && speed > 0.0f) {
    // Simple movement - you can implement more complex movement patterns here
    float distance = speed * speedMultiplier * deltaTime;

    // For now, just move in a simple pattern
    // You can implement more complex movement logic here
    trail[0].x += distance * 0.1f;   // Move slowly to the right
    trail[0].y += distance * 0.05f;  // Move slowly down

    // Update trail
    if (trail.size() > 1) {
      for (size_t i = trail.size() - 1; i > 0; --i) {
        trail[i] = trail[i - 1];
      }
    }
  }
}

void PointEntity::render(SDL_Renderer* renderer) {
  if (!visible || trail.empty()) return;

  if (!trailProps.enabled) {
    // Render just the current point
    SDL_SetRenderDrawColor(renderer, trailProps.startColor.r,
                           trailProps.startColor.g, trailProps.startColor.b,
                           trailProps.startColor.a);
    SDL_RenderPoint(renderer, trail[0].x, trail[0].y);
    return;
  }

  // Render trail with gradient
  for (size_t i = 0; i < trail.size(); ++i) {
    float progress =
        static_cast<float>(i) / static_cast<float>(trail.size() - 1);

    // Apply fade range
    if (progress < trailProps.fadeStart || progress > trailProps.fadeEnd) {
      continue;
    }

    // Normalize progress within fade range
    float normalizedProgress = (progress - trailProps.fadeStart) /
                               (trailProps.fadeEnd - trailProps.fadeStart);

    // Interpolate color
    SDL_Color color;
    color.r = static_cast<Uint8>(
        trailProps.startColor.r +
        (trailProps.endColor.r - trailProps.startColor.r) * normalizedProgress);
    color.g = static_cast<Uint8>(
        trailProps.startColor.g +
        (trailProps.endColor.g - trailProps.startColor.g) * normalizedProgress);
    color.b = static_cast<Uint8>(
        trailProps.startColor.b +
        (trailProps.endColor.b - trailProps.startColor.b) * normalizedProgress);
    color.a = static_cast<Uint8>(
        trailProps.startColor.a +
        (trailProps.endColor.a - trailProps.startColor.a) * normalizedProgress);

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderPoint(renderer, trail[i].x, trail[i].y);
  }
}

void PointEntity::setTrailLength(size_t length) {
  trailLength = length;
  trail.resize(length);
}

void PointEntity::setInitialPosition(float x, float y) {
  for (size_t i = 0; i < trail.size(); ++i) {
    trail[i] = {x - i, y - i};  // Create initial trail
  }
}

void PointEntity::setPosition(const SDL_FPoint& position) {
  if (!trail.empty()) {
    trail[0] = position;
  }
}

SDL_FPoint PointEntity::getPosition() const { return getCurrentPosition(); }
