#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3_ttf/SDL_ttf.h>

class Context {
 public:
  SDL_Window* window;
  SDL_Renderer* renderer;

  Context();
  ~Context();
};
