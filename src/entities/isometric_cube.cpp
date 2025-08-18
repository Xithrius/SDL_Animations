#include "isometric_cube.h"

#include "SDL3/SDL_render.h"

const int LINE_COUNT = 11;

void IsometricCubeEntity::render(SDL_Renderer* renderer) {
  if (!visible) return;

  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

  float x = current_position.x;
  float y = current_position.y;
  float halfsize = size / 2.0f;

  SDL_FPoint lines[LINE_COUNT] = {{x, y},
                                  {x - size, y - halfsize},
                                  {x - size, y - size - halfsize},
                                  {x, y - size},
                                  {x, y},
                                  {x + size, y - halfsize},
                                  {x + size, y - size - halfsize},
                                  {x, y - size},
                                  {x + size, y - size - halfsize},
                                  {x, y - size * 2},
                                  {x - size, y - size - halfsize}};

  SDL_RenderLines(renderer, lines, LINE_COUNT);
}
