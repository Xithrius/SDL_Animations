#include <SDL3/SDL.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>

#include <memory>
#include <string>
#include <vector>

#include "constants.h"
#include "context.h"

void createPoints(AppContext* context, float deltaTime, int current_width,
                  int current_height) {
  for (size_t i = 0; i < context->points.size(); i++) {
    auto& points = context->points[i];
    for (size_t j = 0; j < points.size(); j++) {
      points[j].x += context->point_speeds[i] * deltaTime;
      points[j].y += context->point_speeds[i] * deltaTime;

      if (points[j].x > current_width) {
        points[j].x = 0;
      }
      if (points[j].y > current_height) {
        points[j].y = 0;
      }
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

  const int frameDelay = 1000 / TARGET_FRAME_RATE;

  Uint32 frameStart;
  Uint32 lastFrameTime = SDL_GetTicks();
  int frameTime;

  // Main game loop
  bool running = true;
  SDL_Event event;

  while (running) {
    frameStart = SDL_GetTicks();

    // Calculate delta time in seconds
    Uint32 currentFrameTime = SDL_GetTicks();
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
    SDL_SetRenderDrawColor(context->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
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
    std::string fps_text = "FPS: " + std::to_string(static_cast<int>(fps));
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
