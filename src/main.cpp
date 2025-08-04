#include <SDL3/SDL_log.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <string>
#include <vector>

#include "constants.h"
#include "context.h"

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  SDL_SetAppMetadata(APPLICATION_TITLE.c_str(), VERSION_STRING.c_str(),
                     APPLICATION_IDENTIFIER.c_str());

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  try {
    *appstate = new AppContext();
  } catch (const std::exception &e) {
    SDL_Log("AppContext construction failed: %s", e.what());
    return SDL_APP_FAILURE;
  }

  return SDL_APP_CONTINUE;
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS; /* End the program, reporting success to the OS. */
  }

  return SDL_APP_CONTINUE;
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate) {
  AppContext *context = static_cast<AppContext *>(appstate);

  float delta_seconds = context->updateFrameTiming();

  // Get current window dimensions
  int current_width, current_height;
  SDL_GetWindowSize(context->window, &current_width, &current_height);

  for (size_t i = 0; i < context->points.size(); i++) {
    auto &points = context->points[i];
    for (size_t j = 0; j < points.size(); j++) {
      points[j].x += context->point_speeds[i] * delta_seconds;
      points[j].y += context->point_speeds[i] * delta_seconds;

      if (points[j].x > current_width) {
        points[j].x = 0;
      }
      if (points[j].y > current_height) {
        points[j].y = 0;
      }
    }
  }

  // Clear the screen with black background
  SDL_SetRenderDrawColor(context->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(context->renderer);

  for (int j = 0; j < TRAIL_LENGTH; j++) {
    // Calculate gradient: white (255) to black (0) based on trail position
    int color_value = 255 - (j * 255 / (TRAIL_LENGTH - 1));

    SDL_SetRenderDrawColor(context->renderer, color_value, color_value,
                           color_value, SDL_ALPHA_OPAQUE);

    // Collect all points at this trail position
    std::vector<SDL_FPoint> column_points;
    column_points.reserve(context->points.size());

    for (size_t i = 0; i < context->points.size(); i++) {
      column_points.push_back(context->points[i][j]);
    }

    // Render all points at this trail position in one batch
    SDL_RenderPoints(context->renderer, column_points.data(),
                     column_points.size());
  }

  // Render some text to demonstrate SDL_ttf
  SDL_Color text_color = {255, 255, 255, 255};  // White text
  context->renderText("SDL Animations with Text Rendering", 10, 10, text_color);

  // Render FPS counter
  std::string fps_text = "FPS: " + std::to_string((int)(1.0f / delta_seconds));
  context->renderText(fps_text, 10, 40, text_color);

  // Render point count
  std::string points_text = "Points: " + std::to_string(context->points.size());
  context->renderText(points_text, 10, 70, text_color);

  SDL_RenderPresent(context->renderer);

  return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  AppContext *context = static_cast<AppContext *>(appstate);
  delete context;
}
