#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <vector>

#include "graphics/fonts.h"
#include "imgui.h"

class AppContext {
 public:
  SDL_Window* window;
  SDL_Renderer* renderer;
  FontRef font;
  std::vector<std::vector<SDL_FPoint>> points;
  std::vector<float> point_speeds;
  float target_frame_rate;
  ImGuiIO* io;

  AppContext();
  ~AppContext();
};
