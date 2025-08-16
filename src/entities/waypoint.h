#pragma once

#include <SDL3/SDL.h>

#include "SDL3/SDL_rect.h"
#include "entity.h"

class AppState;

class WaypointEntity : public Entity, public IPositionable, public IUpdatable {
 public:
 private:
  AppState* appState;

  SDL_FPoint position0 = {0, 0};
  SDL_FPoint position1;

  SDL_FPoint current_position;
  float movementSpeed = 2.0f;  // Speed of the sine wave movement

  float time = 0.0f;

  // Helper method to generate random position
  void generateRandomPosition();

 public:
  WaypointEntity(AppState* appState);

  void update(float deltaTime) override;

  void render(SDL_Renderer* renderer) override;

  // Entity type identification
  EntityType getEntityType() const override {
    static EntityType typeId = EntityTypeRegistry::registerType("Waypoint");
    return typeId;
  }

  IPositionable* asPositionable() override { return this; }

  IUpdatable* asUpdatable() override { return this; }

  void setPosition(const SDL_FPoint& position) override;
  SDL_FPoint getPosition() const override;

  void setInitialPosition(float x, float y);

  void regenerateRandomPosition();

  SDL_FPoint getCurrentPosition() const { return current_position; }

  // Movement control
  void setMovementSpeed(float speed) { movementSpeed = speed; }

  float getMovementSpeed() const { return movementSpeed; }

  // Reset animation cycle to start from position0
  void resetAnimation() { time = 0.0f; }

  void regenerateRandomPositionWithDistance();
};
