#pragma once

#include <SDL3/SDL.h>

#include <vector>

#include "entity.h"

// Forward declaration
class AppState;

// Point entity with trail support
class PointEntity : public Entity {
 public:
  // Trail rendering properties
  struct TrailProperties {
    bool enabled = true;
    float fadeStart = 0.0f;  // 0.0 = start of trail, 1.0 = end of trail
    float fadeEnd = 1.0f;    // 0.0 = start of trail, 1.0 = end of trail
    SDL_Color startColor = {255, 255, 255, 255};
    SDL_Color endColor = {0, 0, 0, 0};
  };

 private:
  AppState* appState;
  std::vector<SDL_FPoint> trail;
  size_t trailLength;
  float speed;
  float speedMultiplier = 1.0f;

  TrailProperties trailProps;

 public:
  PointEntity(AppState* appState, size_t trailLength = 10,
              float speed = 100.0f);

  void update(float deltaTime) override;

  void render(SDL_Renderer* renderer) override;

  // Entity type identification
  EntityType getEntityType() const override {
    static EntityType typeId = EntityTypeRegistry::registerType("Point");
    return typeId;
  }

  // Point-specific methods
  void setSpeed(float speed) { this->speed = speed; }

  void setSpeedMultiplier(float multiplier) { speedMultiplier = multiplier; }

  void setTrailLength(size_t length);

  void setTrailProperties(const TrailProperties& props) { trailProps = props; }

  void setInitialPosition(float x, float y);

  const std::vector<SDL_FPoint>& getTrail() const { return trail; }

  SDL_FPoint getCurrentPosition() const {
    return trail.empty() ? SDL_FPoint{0, 0} : trail[0];
  }
};
