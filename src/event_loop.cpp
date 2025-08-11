#include "event_loop.h"

#include <spdlog/spdlog.h>

#include "imgui_impl_sdl3.h"

EventLoop::EventLoop() {
  try {
    this->context = std::make_unique<AppContext>();
    this->graphics = std::make_unique<Graphics>(this->context.get());
  } catch (const std::exception& e) {
    SPDLOG_ERROR("EventLoop construction failed: %s", e.what());
    throw;
  }
}

/**
 * @brief Runs the event loop.
 */
void EventLoop::run() {
  SPDLOG_INFO("Started event loop");

  float lastTime = SDL_GetTicks() / 1000.0f;

  while (this->running) {
    float currentTime = SDL_GetTicks() / 1000.0f;
    float deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    // Cap delta time to prevent spiral of death
    if (deltaTime > 0.25f) {
      deltaTime = 0.25f;
      SPDLOG_WARN("Large delta time detected: {}, capped to 0.25", deltaTime);
    }

    this->HandleInputEvents();

    this->accumulator += deltaTime;
    while (this->accumulator >= FIXED_TIMESTEP) {
      this->updateEvents(FIXED_TIMESTEP);
      this->accumulator -= FIXED_TIMESTEP;
    }

    this->updateFPS(deltaTime);

    this->graphics->render(fps, getTickRate());

    if (this->targetFPS > 0) {
      float frameTime = 1.0f / this->targetFPS;
      float elapsed = (SDL_GetTicks() / 1000.0f) - currentTime;
      if (elapsed < frameTime) {
        SDL_Delay(static_cast<Uint32>((frameTime - elapsed) * 1000));
      }
    }
  }
}

/**
 * @brief Handles all input events (keyboard, mouse, etc.).
 */
void EventLoop::HandleInputEvents() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    ImGui_ImplSDL3_ProcessEvent(&event);
    if (event.type == SDL_EVENT_QUIT) this->running = false;
    if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
        event.window.windowID == SDL_GetWindowID(this->context->window))
      this->running = false;
  }
}

/**
 * @brief All event update logic goes here, nothing should be rendered.
 *
 * @param deltaTime The time since the last update.
 */
void EventLoop::updateEvents(float deltaTime) {
  this->graphics->updatePoints(deltaTime);
}

void EventLoop::updateFPS(float deltaTime) {
  this->frameCount++;
  this->fpsUpdateTimer += deltaTime;

  if (this->fpsUpdateTimer >= 1.0f) {
    this->fps = static_cast<float>(this->frameCount) / this->fpsUpdateTimer;
    this->frameCount = 0;
    this->fpsUpdateTimer = 0.0f;
  }
}
