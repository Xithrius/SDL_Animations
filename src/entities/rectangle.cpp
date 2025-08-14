#include "rectangle.h"

#include <spdlog/spdlog.h>

RectangleEntity::RectangleEntity(const SDL_FRect& rect) : rect(rect) {}

void RectangleEntity::update(float) {}

void RectangleEntity::render(SDL_Renderer* renderer) {
  if (!visible) return;

  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

  if (filled) {
    SDL_RenderFillRect(renderer, &rect);
  } else {
    // Draw border by drawing 4 lines
    SDL_FPoint topLeft = {rect.x, rect.y};
    SDL_FPoint topRight = {rect.x + rect.w, rect.y};
    SDL_FPoint bottomLeft = {rect.x, rect.y + rect.h};
    SDL_FPoint bottomRight = {rect.x + rect.w, rect.y + rect.h};

    SDL_RenderLine(renderer, topLeft.x, topLeft.y, topRight.x, topRight.y);
    SDL_RenderLine(renderer, topRight.x, topRight.y, bottomRight.x,
                   bottomRight.y);
    SDL_RenderLine(renderer, bottomRight.x, bottomRight.y, bottomLeft.x,
                   bottomLeft.y);
    SDL_RenderLine(renderer, bottomLeft.x, bottomLeft.y, topLeft.x, topLeft.y);
  }
}
