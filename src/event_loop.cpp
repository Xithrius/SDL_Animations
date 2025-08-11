#include "event_loop.h"

#include <SDL3/SDL_log.h>

#include "constants.h"

EventLoop::EventLoop() {
  try {
    context = std::make_unique<AppContext>();
    graphics = std::make_unique<Graphics>(context.get());
  } catch (const std::exception& e) {
    SDL_Log("EventLoop construction failed: %s", e.what());
    throw;
  }
}

EventLoop::~EventLoop() = default;

/**
 * @brief Runs the event loop.
 */
void EventLoop::run() {
  float lastTime = SDL_GetTicks() / 1000.0f;

  while (running) {
    float currentTime = SDL_GetTicks() / 1000.0f;
    float deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    // Cap delta time to prevent spiral of death
    if (deltaTime > 0.25f) {
      deltaTime = 0.25f;
      SDL_Log("Large delta time detected: %f, capped to 0.25", deltaTime);
    }

    // Handle events
    HandleInputEvents();

    // Accumulate time for fixed timestep
    accumulator += deltaTime;

    // Run fixed timestep updates (event logic)
    while (accumulator >= FIXED_TIMESTEP) {
      updateEvents(FIXED_TIMESTEP);
      accumulator -= FIXED_TIMESTEP;
    }

    updateFPS(deltaTime);

    render();

    if (targetFPS > 0) {
      float frameTime = 1.0f / targetFPS;
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
    if (event.type == SDL_EVENT_QUIT) {
      this->running = false;
      break;
    }
  }
}

/**
 * @brief All event update logic goes here, nothing should be rendered.
 *
 * @param deltaTime The time since the last update.
 */
void EventLoop::updateEvents(float deltaTime) {
  this->graphics->renderPoints(deltaTime);
}

/**
 * @brief Renders all entities in the scene.
 */
void EventLoop::render() {
  SDL_RenderClear(this->context->renderer);

  if (this->context->points.empty()) {
    return;
  }

  for (int j = 0; j < TRAIL_LENGTH; j++) {
    // Calculate gradient: white (255) to black (0) based on trail position
    int color_value = 255 - (j * 255 / (TRAIL_LENGTH - 1));

    SDL_SetRenderDrawColor(this->context->renderer, color_value, color_value,
                           color_value, SDL_ALPHA_OPAQUE);

    std::vector<SDL_FPoint> column_points;
    column_points.reserve(this->context->points.size());

    for (size_t i = 0; i < this->context->points.size(); i++) {
      // Check if the point trail is properly initialized
      SDL_FPoint column_point = this->context->points[i][j];
      column_points.push_back(column_point);
    }

    SDL_RenderPoints(this->context->renderer, column_points.data(),
                     column_points.size());
  }

  // Render UI text
  SDL_Color text_color = {255, 255, 255, 255};
  this->graphics->renderText("SDL Animations with Fixed Timestep", 10, 10,
                             text_color);

  std::string fps_text = "FPS: " + std::to_string(static_cast<int>(fps));
  this->graphics->renderText(fps_text, 10, 40, text_color);

  std::string tick_text =
      "Event Logic: " + std::to_string(static_cast<int>(getTickRate())) + " Hz";
  this->graphics->renderText(tick_text, 10, 70, text_color);

  std::string points_text =
      "Points: " + std::to_string(this->context->points.size());
  this->graphics->renderText(points_text, 10, 100, text_color);

  SDL_RenderPresent(this->context->renderer);
}

void EventLoop::updateFPS(float deltaTime) {
  frameCount++;
  fpsUpdateTimer += deltaTime;

  // Update FPS every second
  if (fpsUpdateTimer >= 1.0f) {
    fps = static_cast<float>(frameCount) / fpsUpdateTimer;
    frameCount = 0;
    fpsUpdateTimer = 0.0f;
  }
}
