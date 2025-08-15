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
  if (!active) return;

  int windowWidth, windowHeight;
  SDL_GetWindowSize(appState->context->window, &windowWidth, &windowHeight);

  // Calculate new position
  double currentSpeed = static_cast<double>(speed) * speedMultiplier;
  double newX = static_cast<double>(trail[0].x) +
                currentSpeed * static_cast<double>(deltaTime);
  double newY = static_cast<double>(trail[0].y) +
                currentSpeed * static_cast<double>(deltaTime);

  // Wrap around screen
  newX = fmod(newX, static_cast<double>(windowWidth));
  newY = fmod(newY, static_cast<double>(windowHeight));

  // Shift trail and add new position
  for (size_t i = trail.size() - 1; i > 0; --i) {
    trail[i] = trail[i - 1];
  }
  trail[0] = {static_cast<float>(newX), static_cast<float>(newY)};
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
