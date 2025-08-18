#include "isometric_cube.h"

#include "core/app_state.h"

IsometricCubeEntity::IsometricCubeEntity(AppState* appState)
    : appState(appState) {}

void IsometricCubeEntity::render(SDL_Renderer* renderer) {
  if (!visible) return;

  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderPoint(renderer, current_position.x, current_position.y);
}
