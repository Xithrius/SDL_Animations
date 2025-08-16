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
    SDL_RenderRect(renderer, &rect);
  }
}

BoundingBox RectangleEntity::getBoundingBox() const {
  return BoundingBox(rect.x, rect.y, rect.x + rect.w, rect.y + rect.h);
}

void RectangleEntity::setPosition(const SDL_FPoint& position) {
  // Update rectangle position while maintaining size
  float width = rect.w;
  float height = rect.h;
  rect.x = position.x - width / 2.0f;  // Center the rectangle on the position
  rect.y = position.y - height / 2.0f;
}

SDL_FPoint RectangleEntity::getPosition() const {
  return SDL_FPoint{rect.x + rect.w / 2.0f, rect.y + rect.h / 2.0f};
}
