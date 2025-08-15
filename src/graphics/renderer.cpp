#include "renderer.h"

#include "spdlog/spdlog.h"

void Renderer::renderText(const std::string& text, int x, int y,
                          SDL_Color color) {
  // Check if font is loaded
  if (!this->font.get()) {
    SPDLOG_WARN("Font not loaded, skipping text rendering");
    return;
  }

  SDL_Surface* text_surface = TTF_RenderText_Blended(
      this->font.get(), text.c_str(), text.length(), color);
  if (!text_surface) {
    SPDLOG_ERROR("Couldn't render text: %s", SDL_GetError());
    return;
  }

  SDL_Texture* text_texture =
      SDL_CreateTextureFromSurface(this->renderer, text_surface);
  if (!text_texture) {
    SPDLOG_ERROR("Couldn't create texture from surface: %s", SDL_GetError());
    SDL_DestroySurface(text_surface);
    return;
  }

  SDL_FRect dest_rect = {(float)x, (float)y, (float)text_surface->w,
                         (float)text_surface->h};
  SDL_RenderTexture(this->renderer, text_texture, nullptr, &dest_rect);

  SDL_DestroySurface(text_surface);
  SDL_DestroyTexture(text_texture);
}

void Renderer::clear() { SDL_RenderClear(renderer); }

void Renderer::present() { SDL_RenderPresent(renderer); }
