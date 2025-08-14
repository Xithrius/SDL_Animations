#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <spdlog/spdlog.h>

#include "core/app_state.h"
#include "ui/debug.h"

// Forward declaration to avoid circular dependency
class EventLoop;

static ImVec4 CLEAR_COLOR = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);

class UI {
 private:
  AppState* appState;
  EventLoop* eventLoop;

 public:
  DebugUI debug;

  UI(AppState* appState, EventLoop* eventLoop)
      : appState(appState), eventLoop(eventLoop), debug(appState, this) {}

  ~UI() = default;

  void render();

  void createDemoEntities();

  void renderDebugFrames();

  EventLoop* getEventLoop() const { return eventLoop; }
};
