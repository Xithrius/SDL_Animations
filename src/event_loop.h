#pragma once

#include <SDL3/SDL.h>

#include <memory>
#include <string>
#include <vector>

#include "core/app_state.h"
#include "core/context.h"
#include "entities/entity.h"
#include "ui/ui.h"

class EventLoop {
 private:
  std::unique_ptr<Context> context;
  std::unique_ptr<AppState> appState;
  std::unique_ptr<UI> ui;

  // Fixed timestep for event logic (60 Hz)
  static constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;
  float accumulator = 0.0f;

  float targetFPS = 60.0f;

  bool running = true;

  float lastFrameTime = 0.0f;
  float fps = 0.0f;
  int frameCount = 0;
  float fpsUpdateTimer = 0.0f;

 public:
  EventLoop();
  ~EventLoop() = default;

  void run();

  void stop() { running = false; }

  // Set target rendering FPS (0 = unlimited)
  void setTargetFPS(float fps) { targetFPS = fps; }

  // Get target FPS
  float getTargetFPS() const { return targetFPS; }

  // Get current FPS
  float getFPS() const { return fps; }

 private:
  void HandleInputEvents();
  void updateEvents(float deltaTime);
  void updateFPS(float deltaTime);
  void render();
  void renderDebugFrames();
  void renderDebugInfo(Entity* entity);
  std::vector<std::string> getDebugText(Entity* entity);
};
