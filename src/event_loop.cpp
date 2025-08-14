#include "event_loop.h"

#include <spdlog/spdlog.h>

#include "imgui_impl_sdl3.h"

EventLoop::EventLoop() {
  try {
    this->context = std::make_unique<Context>();
    this->appState = std::make_unique<AppState>(this->context.get());
    this->ui = std::make_unique<UI>(this->appState.get());
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

  Uint64 freq = SDL_GetPerformanceFrequency();
  Uint64 last = SDL_GetPerformanceCounter();
  double targetFrameTime = 1.0 / this->targetFPS;

  while (this->running) {
    Uint64 now = SDL_GetPerformanceCounter();
    double deltaTime = (double)(now - last) / freq;
    last = now;

    this->HandleInputEvents();

    this->accumulator += deltaTime;
    while (this->accumulator >= FIXED_TIMESTEP) {
      this->updateEvents(FIXED_TIMESTEP);
      this->accumulator -= FIXED_TIMESTEP;
    }

    this->updateFPS(deltaTime);

    this->ui->render();

    double elapsed = (double)(SDL_GetPerformanceCounter() - now) / freq;
    if (elapsed < targetFrameTime) {
      double sleepTime = targetFrameTime - elapsed;
      if (sleepTime > 0.001) {  // Only sleep if we have more than 1ms to wait
        SDL_Delay((Uint32)(sleepTime * 1000.0 + 0.5));
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
    if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_F3) {
      this->ui->debug.toggle();
    }
  }
}

/**
 * @brief All event update logic goes here, nothing should be rendered.
 *
 * @param deltaTime The time since the last update.
 */
void EventLoop::updateEvents(float deltaTime) {
  this->appState->entityManager.update(deltaTime);
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
