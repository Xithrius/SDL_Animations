#include <SDL3/SDL.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>

#include <cmath>
#include <memory>
#include <string>
#include <vector>

#include "constants.h"
#include "context.h"

void createPoints(AppContext* context, float deltaTime, int current_width,
                  int current_height) {
  for (size_t i = 0; i < context->points.size(); i++) {
    auto& points = context->points[i];
    double speed = static_cast<double>(context->point_speeds[i]);

    for (size_t j = 0; j < points.size(); j++) {
      // Use double precision for calculations to avoid floating-point drift
      double newX = static_cast<double>(points[j].x) +
                    speed * static_cast<double>(deltaTime);
      double newY = static_cast<double>(points[j].y) +
                    speed * static_cast<double>(deltaTime);

      // Use modulo for wrapping to maintain precision
      newX = fmod(newX, static_cast<double>(current_width));
      newY = fmod(newY, static_cast<double>(current_height));

      // Convert back to float for rendering
      points[j].x = static_cast<float>(newX);
      points[j].y = static_cast<float>(newY);
    }
  }
}

int main() {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
    return 1;
  }

  // Set application metadata
  SDL_SetAppMetadata(APPLICATION_TITLE.c_str(), VERSION_STRING.c_str(),
                     APPLICATION_IDENTIFIER.c_str());

  // Create application context using smart pointer
  std::unique_ptr<AppContext> context;
  try {
    context = std::make_unique<AppContext>();
  } catch (const std::exception& e) {
    SDL_Log("AppContext construction failed: %s", e.what());
    SDL_Quit();
    return 1;
  }

  const float frameDelay = 1000.0f / TARGET_FRAME_RATE;

  float frameStart;
  float lastFrameTime = SDL_GetTicks();
  float frameTime;

  // Main game loop
  bool running = true;
  SDL_Event event;

  while (running) {
    frameStart = SDL_GetTicks();

    float currentFrameTime = SDL_GetTicks();
    float deltaTime = (currentFrameTime - lastFrameTime) / 1000.0f;
    lastFrameTime = currentFrameTime;

    // Handle events
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        running = false;
        break;
      }
    }

    int current_width, current_height;
    SDL_GetWindowSize(context->window, &current_width, &current_height);

    createPoints(context.get(), deltaTime, current_width, current_height);

    // Clear the screen with black background
    SDL_RenderClear(context->renderer);

    for (int j = 0; j < TRAIL_LENGTH; j++) {
      // Calculate gradient: white (255) to black (0) based on trail position
      int color_value = 255 - (j * 255 / (TRAIL_LENGTH - 1));

      SDL_SetRenderDrawColor(context->renderer, color_value, color_value,
                             color_value, SDL_ALPHA_OPAQUE);

      std::vector<SDL_FPoint> column_points;
      column_points.reserve(context->points.size());

      for (size_t i = 0; i < context->points.size(); i++) {
        column_points.push_back(context->points[i][j]);
      }

      SDL_RenderPoints(context->renderer, column_points.data(),
                       column_points.size());
    }

    SDL_Color text_color = {255, 255, 255, 255};  // White text
    context->renderText("SDL Animations with Text Rendering", 10, 10,
                        text_color);

    // Display FPS in text
    float fps = (deltaTime > 0.0f) ? (1.0f / deltaTime) : 0.0f;
    std::string fps_text = "FPS: " + std::to_string(static_cast<float>(fps));
    context->renderText(fps_text, 10, 40, text_color);

    std::string points_text =
        "Points: " + std::to_string(context->points.size());
    context->renderText(points_text, 10, 70, text_color);

    SDL_RenderPresent(context->renderer);

    frameTime = SDL_GetTicks() - frameStart;

    if (frameDelay > frameTime) {
      SDL_Delay(frameDelay - frameTime);
    }
  }

  SDL_Quit();

  return 0;
}
