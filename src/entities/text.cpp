#include "text.h"

#include "core/app_state.h"

TextEntity::TextEntity(const std::string& text, int x, int y, SDL_Color color)
    : text(text), x(x), y(y), color(color) {}

void TextEntity::update(float) {}

void TextEntity::render(SDL_Renderer*) {
  if (appState) {
    appState->renderer.renderText(text, x, y, color);
  }
}

BoundingBox TextEntity::getBoundingBox() const {
  float width = text.length() * 8.0f;  // Rough estimate: 8 pixels per character
  float height = 16.0f;                // Rough estimate: 16 pixels height

  return BoundingBox(x, y, x + width, y + height);
}
