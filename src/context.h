#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>

#include <vector>

#include "constants.h"

class AppContext {
 public:
  SDL_Window *window;
  SDL_Renderer *renderer;
  std::vector<std::vector<SDL_FPoint>> points;
  std::vector<float> point_speeds;
  Uint64 last_frame_time;
  float target_frame_rate;
  float target_frame_time;

  AppContext();

  float updateFrameTiming();
};
