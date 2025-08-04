#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <vector>

#include "constants.h"

class AppContext {
 public:
  SDL_Window *window;
  SDL_Renderer *renderer;
  TTF_Font *font;
  std::vector<std::vector<SDL_FPoint>> points;
  std::vector<float> point_speeds;
  Uint64 last_frame_time;
  float target_frame_rate;
  float target_frame_time;

  AppContext();
  ~AppContext();

  float updateFrameTiming();
  void renderText(const std::string &text, int x, int y,
                  SDL_Color color = {255, 255, 255, 255});
};
