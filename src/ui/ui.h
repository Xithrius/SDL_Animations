#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <spdlog/spdlog.h>

#include "core/app_state.h"
#include "ui/debug.h"

static ImVec4 CLEAR_COLOR = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);

class UI {
 private:
  AppState* appState;

 public:
  DebugUI debug;

  UI(AppState* appState) : appState(appState), debug(appState, this) {}

  ~UI() = default;

  void render();

  void createDemoEntities();
};
