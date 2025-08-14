#pragma once

#include <SDL3/SDL.h>

#include <string>

class Renderer {
 private:
  SDL_Renderer* renderer;

 public:
  Renderer(SDL_Renderer* renderer) : renderer(renderer) {}

  void renderText(const std::string& text, int x, int y,
                  SDL_Color color = {255, 255, 255, 255});

  void clear();
  void present();

  SDL_Renderer* getRenderer() const { return renderer; }
};
