#include "SDL3/SDL_render.h"

static const int INDICES_LENGTH = 6;
static const int INDICES[INDICES_LENGTH] = {0, 1, 2, 0, 2, 3};

static const int POINTS_LENGTH = 4;

void renderQuadGeometry(SDL_Renderer* renderer, SDL_FPoint* points,
                        SDL_FColor color) {
  SDL_Vertex vertices[POINTS_LENGTH];

  for (int i = 0; i < POINTS_LENGTH; i++) {
    vertices[i].position = points[i];
    vertices[i].color = color;
  }

  SDL_RenderGeometry(renderer, NULL, vertices, POINTS_LENGTH, INDICES,
                     INDICES_LENGTH);
}
