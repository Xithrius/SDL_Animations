#include "SDL3/SDL_render.h"
#include "entities/utils/quad_geometry.h"
#include "isometric_cube.h"

const int ISOMETRIC_CUBE_LINE_COUNT = 11;
const int ISOMETRIC_QUAD_POINTS_LENGTH = 4;

void IsometricCubeEntity::render(SDL_Renderer* renderer) {
  if (!visible) return;

  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

  float x = current_position.x;
  float y = current_position.y;
  float halfsize = size / 2.0f;

  // Top
  SDL_FColor color = {0.4f, 0.4f, 0.4f, 1.0f};
  SDL_FPoint points[ISOMETRIC_QUAD_POINTS_LENGTH] = {
      {x - size, y - size - halfsize},
      {x, y - size * 2},
      {x + size, y - size - halfsize},
      {x, y - size}};
  renderQuadGeometry(renderer, points, color);

  // Left
  color = {0.3f, 0.3f, 0.3f, 1.0f};
  SDL_FPoint leftPoints[ISOMETRIC_QUAD_POINTS_LENGTH] = {
      {x - size, y - halfsize},
      {x - size, y - size - halfsize},
      {x, y - size},
      {x, y}};
  renderQuadGeometry(renderer, leftPoints, color);

  // Right
  color = {0.5f, 0.5f, 0.5f, 1.0f};
  SDL_FPoint rightPoints[ISOMETRIC_QUAD_POINTS_LENGTH] = {
      {x, y},
      {x, y - size},
      {x + size, y - size - halfsize},
      {x + size, y - halfsize}};
  renderQuadGeometry(renderer, rightPoints, color);

  SDL_FPoint lines[ISOMETRIC_CUBE_LINE_COUNT] = {
      {x, y},
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
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
  SDL_RenderLines(renderer, lines, ISOMETRIC_CUBE_LINE_COUNT);
}
