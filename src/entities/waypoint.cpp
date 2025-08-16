#include "waypoint.h"

#include <spdlog/spdlog.h>

#include <cmath>
#include <random>

#include "core/app_state.h"

static std::mt19937 rng(std::random_device{}());
static std::uniform_real_distribution<float> dist(0.0f, 1.0f);

void WaypointEntity::generateRandomPosition() {
  // Generate a random angle between 0 and 2π
  float randomAngle = dist(rng) * 2.0f * M_PI;
  float randomDistance = dist(rng) * 500.0f;  // Random distance up to 500

  position1.x = position0.x + cos(randomAngle) * randomDistance;
  position1.y = position0.y + sin(randomAngle) * randomDistance;
}

WaypointEntity::WaypointEntity(AppState* appState) : appState(appState) {
  generateRandomPosition();
  current_position = position0;
}

void WaypointEntity::update(float deltaTime) {
  time += deltaTime * movementSpeed;

  // Use sine wave to interpolate between position0 and position1
  // We want to start at position0 (t=0) and move to position1 (t=1)
  // sin(time) goes from -1 to 1, so we map it to 0 to 1 for interpolation
  // Start with sin(time - π/2) so that at time=0, we get sin(-π/2) = -1, which
  // maps to t=0
  float t = (sin(time - M_PI / 2.0f) + 1.0f) /
            2.0f;  // Maps sin to [0,1] range starting at 0

  // Interpolate between position0 and position1
  current_position.x = position0.x + t * (position1.x - position0.x);
  current_position.y = position0.y + t * (position1.y - position0.y);
}

void WaypointEntity::render(SDL_Renderer* renderer) {
  if (!visible) return;

  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderPoint(renderer, current_position.x, current_position.y);
}

void WaypointEntity::setPosition(const SDL_FPoint& position) {
  current_position = position;
}

void WaypointEntity::setInitialPosition(float x, float y) {
  position0.x = x;
  position0.y = y;
  current_position = position0;

  // Regenerate the random position relative to the new center
  regenerateRandomPosition();
}

void WaypointEntity::regenerateRandomPosition() {
  generateRandomPosition();

  // Reset animation cycle to start from position0
  time = 0.0f;
}

void WaypointEntity::regenerateRandomPositionWithDistance() {
  generateRandomPosition();

  // Reset animation cycle to start from position0
  time = 0.0f;
}

SDL_FPoint WaypointEntity::getPosition() const { return current_position; }
