#pragma once

#include <SDL3/SDL.h>

#include "SDL3/SDL_rect.h"
#include "entities/entity.h"

class AppState;

class IsometricCubeEntity : public Entity,
                            public IPositionable,
                            public IUpdatable {
 private:
  AppState* appState;

  SDL_FPoint current_position = {0, 0};

  SDL_FPoint position0;
  SDL_FPoint position1;

  float wave_dy = 25.0f;

  float size = 50.0f;

  float movementSpeed = 2.0f;
  float time = 0.0f;

 public:
  IsometricCubeEntity(AppState* appState) : appState(appState) {}

  void update(float) override;

  void render(SDL_Renderer* renderer) override;

  // Entity type identification
  EntityType getEntityType() const override {
    static EntityType typeId =
        EntityTypeRegistry::getInstance().registerType("IsometricCube");
    return typeId;
  }

  void setTime(const float newTime) { time = newTime; }

  void setPosition(const SDL_FPoint& position) override;

  SDL_FPoint getPosition() const override { return current_position; }

  BoundingBox getBoundingBox() const override;

  SDL_FPoint getBehindLeft();
  SDL_FPoint getBehindRight();
  SDL_FPoint getFrontLeft();
  SDL_FPoint getFrontRight();
};
