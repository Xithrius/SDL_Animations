#pragma once

#include <SDL3/SDL.h>

#include <memory>

#include "context.h"
#include "graphics/graphics.h"

class EventLoop {
 private:
  std::unique_ptr<AppContext> context;
  std::unique_ptr<Graphics> graphics;

  // Fixed timestep for event logic (60 Hz)
  static constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;
  float accumulator = 0.0f;

  // Rendering can be variable rate
  float targetFPS = 144.0f;  // Can be different from event logic rate

  bool running = true;

  // Performance tracking
  float lastFrameTime = 0.0f;
  float fps = 0.0f;
  int frameCount = 0;
  float fpsUpdateTimer = 0.0f;

 public:
  EventLoop();
  ~EventLoop();

  void run();

  void stop() { running = false; }

  // Set target rendering FPS (0 = unlimited)
  void setTargetFPS(float fps) { targetFPS = fps; }

  // Get current FPS
  float getFPS() const { return fps; }

  // Get event logic tick rate
  float getTickRate() const { return 1.0f / FIXED_TIMESTEP; }

 private:
  void HandleInputEvents();
  void updateEvents(float deltaTime);
  void render();
  void updateFPS(float deltaTime);
};
