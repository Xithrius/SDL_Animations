#include "renderer.h"

void Renderer::renderText(const std::string& text, int x, int y,
                          SDL_Color color) {
  // TODO: Implement text rendering using fonts
  // For now, this is a placeholder - text rendering will be implemented
  // when we integrate the font system with the renderer
  (void)text;
  (void)x;
  (void)y;
  (void)color;
}

void Renderer::clear() { SDL_RenderClear(renderer); }

void Renderer::present() { SDL_RenderPresent(renderer); }
