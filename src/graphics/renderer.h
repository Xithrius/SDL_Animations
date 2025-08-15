#pragma once

#include <SDL3/SDL.h>

#include <memory>
#include <string>

#include "SDL3_ttf/SDL_ttf.h"

class Renderer {
 private:
  SDL_Renderer* renderer;
  std::shared_ptr<TTF_Font> font;

 public:
  Renderer() : renderer(nullptr), font(nullptr) {}

  Renderer(SDL_Renderer* renderer, std::shared_ptr<TTF_Font> font)
      : renderer(renderer), font(font) {}

  void setRenderer(SDL_Renderer* renderer) { this->renderer = renderer; }

  void setFont(std::shared_ptr<TTF_Font> font) { this->font = font; }

  void renderText(const std::string& text, int x, int y,
                  SDL_Color color = {255, 255, 255, 255});

  void clear();
  void present();

  SDL_Renderer* getRenderer() const { return renderer; }
};
