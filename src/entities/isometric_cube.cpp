#include "isometric_cube.h"

#include "SDL3/SDL_render.h"

void IsometricCubeEntity::render(SDL_Renderer* renderer) {
  if (!visible) return;

  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

  float x = current_position.x;
  float y = current_position.y;
  float halfsize = size / 2.0f;

  // Left line
  SDL_RenderLine(renderer, x - size, y - halfsize, x - size,
                 y - halfsize - size);

  // Middle line
  SDL_RenderLine(renderer, x, y, x, y - size);

  // Right line
  SDL_RenderLine(renderer, x + size, y - halfsize, x + size,
                 y - halfsize - size);

  // Top of middle to top of left
  SDL_RenderLine(renderer, x, y - size, x - size, y - halfsize - size);

  // Top of middle to top of right
  SDL_RenderLine(renderer, x, y - size, x + size, y - halfsize - size);

  // Bottom of middle to Bottom of left
  SDL_RenderLine(renderer, x, y, x - size, y - halfsize);

  // Bottom of middle to Bottom of right
  SDL_RenderLine(renderer, x, y, x + size, y - halfsize);

  // Top of left to above middle line
  SDL_RenderLine(renderer, x - size, y - halfsize - size, x, y - size * 2);

  // Top of right to above middle line
  SDL_RenderLine(renderer, x + size, y - halfsize - size, x, y - size * 2);
}
