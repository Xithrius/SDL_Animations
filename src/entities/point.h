#pragma once

#include <SDL3/SDL.h>

#include <vector>

#include "entity.h"

// Forward declaration
class AppState;

// Point entity with trail support
class PointEntity : public Entity,
                    public IPositionable,
                    public IUpdatable,
                    public IInteractive {
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
  bool draggable = true;  // Can be set to false to disable dragging

  TrailProperties trailProps;

 public:
  PointEntity(AppState* appState, size_t trailLength = 10,
              float speed = 100.0f);

  void update(float deltaTime) override;

  void render(SDL_Renderer* renderer) override;

  // Entity type identification
  EntityType getEntityType() const override {
    static EntityType typeId =
        EntityTypeRegistry::getInstance().registerType("Point");
    return typeId;
  }

  // Interface implementations
  IPositionable* asPositionable() override { return this; }

  IUpdatable* asUpdatable() override { return this; }

  IInteractive* asInteractive() override { return this; }

  // IPositionable implementation
  void setPosition(const SDL_FPoint& position) override;
  SDL_FPoint getPosition() const override;

  // IInteractive implementation
  bool canBeDragged() const override { return draggable; }

  void setDraggable(bool draggable) { this->draggable = draggable; }

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
