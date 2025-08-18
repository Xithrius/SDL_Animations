#include "isometric_cube.h"

#include <cmath>
#include <cstring>

#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"

const int LINE_COUNT = 11;

static const int INDICES_LENGTH = 6;
static const int INDICES[INDICES_LENGTH] = {0, 1, 2, 0, 2, 3};

static const int POINTS_LENGTH = 4;

void renderQuadGeometry(SDL_Renderer* renderer, SDL_FPoint* points,
                        SDL_FColor color) {
  SDL_Vertex vertices[4];

  for (int i = 0; i < POINTS_LENGTH; i++) {
    vertices[i].position = points[i];
    vertices[i].color = color;
  }

  SDL_RenderGeometry(renderer, NULL, vertices, 4, INDICES, INDICES_LENGTH);
}

void IsometricCubeEntity::render(SDL_Renderer* renderer) {
  if (!visible) return;

  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

  float x = current_position.x;
  float y = current_position.y;
  float halfsize = size / 2.0f;

  // Top
  SDL_FColor color = {0.4f, 0.4f, 0.4f, 1.0f};
  SDL_FPoint points[POINTS_LENGTH] = {{x - size, y - size - halfsize},
                                      {x, y - size * 2},
                                      {x + size, y - size - halfsize},
                                      {x, y - size}};
  renderQuadGeometry(renderer, points, color);

  // Left
  color = {0.3f, 0.3f, 0.3f, 1.0f};
  SDL_FPoint leftPoints[POINTS_LENGTH] = {{x - size, y - halfsize},
                                          {x - size, y - size - halfsize},
                                          {x, y - size},
                                          {x, y}};
  renderQuadGeometry(renderer, leftPoints, color);

  // Right
  color = {0.5f, 0.5f, 0.5f, 1.0f};
  SDL_FPoint rightPoints[POINTS_LENGTH] = {{x, y},
                                           {x, y - size},
                                           {x + size, y - size - halfsize},
                                           {x + size, y - halfsize}};
  renderQuadGeometry(renderer, rightPoints, color);

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
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
  SDL_RenderLines(renderer, lines, LINE_COUNT);
}

void IsometricCubeEntity::update(float deltaTime) {
  time += deltaTime * movementSpeed;

  float t = (sin(time - M_PI / 2.0f) + 1.0f) / 2.0f;

  // Interpolate between position0 and position1
  current_position.x = position0.x + t * (position1.x - position0.x);
  current_position.y = position0.y + t * (position1.y - position0.y);
}
