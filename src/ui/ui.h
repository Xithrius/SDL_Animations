#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <spdlog/spdlog.h>

#include "core/app_state.h"
#include "entities/entity.h"
#include "ui/audio_ui.h"
#include "ui/debug.h"
#include "ui/settings.h"

class EventLoop;

static ImVec4 CLEAR_COLOR = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);

class UI {
 private:
  AppState* appState;
  EventLoop* eventLoop;

 public:
  DebugUI debug;
  SettingsUI settings;
  AudioUI audioUI;

  UI(AppState* appState, EventLoop* eventLoop)
      : appState(appState),
        eventLoop(eventLoop),
        debug(appState, this),
        settings(appState, this),
        audioUI() {
    // Set the audio system for the UI
    audioUI.setAudioSystem(appState->audioSystem.get());
  }

  ~UI() = default;

  void render();

  void createDemoEntities();

  EventLoop* getEventLoop() const { return eventLoop; }
};
