#include "event_loop.h"

#include <spdlog/spdlog.h>

#include "constants.h"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

EventLoop::EventLoop() {
  try {
    context = std::make_unique<AppContext>();
    graphics = std::make_unique<Graphics>(context.get());
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

  while (running) {
    float currentTime = SDL_GetTicks() / 1000.0f;
    float deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    // Cap delta time to prevent spiral of death
    if (deltaTime > 0.25f) {
      deltaTime = 0.25f;
      SPDLOG_WARN("Large delta time detected: {}, capped to 0.25", deltaTime);
    }

    this->HandleInputEvents();

    accumulator += deltaTime;
    while (accumulator >= FIXED_TIMESTEP) {
      this->updateEvents(FIXED_TIMESTEP);
      accumulator -= FIXED_TIMESTEP;
    }

    this->updateFPS(deltaTime);

    this->render();

    // Delay if window is minimized
    // if (SDL_GetWindowFlags(this->context->window) & SDL_WINDOW_MINIMIZED) {
    //   SDL_Delay(10);
    //   continue;
    // }

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
  this->graphics->renderPoints(deltaTime);
}

/**
 * @brief Renders all entities in the scene.
 */
void EventLoop::render() {
  SDL_RenderClear(this->context->renderer);

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

  this->graphics->renderText("SDL Animations", 10, 10);

  std::string fps_text = "FPS: " + std::to_string(static_cast<int>(fps));
  this->graphics->renderText(fps_text, 10, 40);

  std::string tick_text =
      "Event Logic: " + std::to_string(static_cast<int>(getTickRate())) + " Hz";
  this->graphics->renderText(tick_text, 10, 70);

  std::string points_text =
      "Points: " + std::to_string(this->context->points.size());
  this->graphics->renderText(points_text, 10, 100);

  // Start the Dear ImGui frame
  ImGui_ImplSDLRenderer3_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  ImGui::NewFrame();

  {
    static int counter = 0;

    ImGui::Begin("Hello, world!");

    // ImGui::SliderInt("Points", this->context->po, 0.0f, 1.0f);
    ImGui::ColorEdit3("clear color", (float*)&this->background_color);

    if (ImGui::Button("Button")) counter++;
    ImGui::SameLine();
    ImGui::Text("counter = %d", counter);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0f / this->context->io->Framerate,
                this->context->io->Framerate);
    ImGui::End();
  }

  ImGui::Render();
  SDL_SetRenderScale(this->context->renderer,
                     this->context->io->DisplayFramebufferScale.x,
                     this->context->io->DisplayFramebufferScale.y);
  SDL_SetRenderDrawColorFloat(this->context->renderer, this->background_color.x,
                              this->background_color.y,
                              this->background_color.z,
                              this->background_color.w);
  ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(),
                                        this->context->renderer);

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
