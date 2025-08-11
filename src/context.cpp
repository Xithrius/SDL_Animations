
#include "context.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <stdexcept>
#include <string>

#include "constants.h"

AppContext::AppContext()
    : window(nullptr),
      renderer(nullptr),
      font(),
      target_frame_rate(TARGET_FRAME_RATE) {
  if (!SDL_CreateWindowAndRenderer(APPLICATION_TITLE.c_str(), WINDOW_WIDTH,
                                   WINDOW_HEIGHT, WINDOW_FLAGS, &window,
                                   &renderer)) {
    SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
    throw std::runtime_error("Failed to create window/renderer");
  }

  // Initialize SDL_ttf
  if (!TTF_Init()) {
    SDL_Log("Couldn't initialize SDL_ttf: %s", SDL_GetError());
    throw std::runtime_error("Failed to initialize SDL_ttf");
  }

  // Load font after SDL_ttf is initialized
  font.loadFont();

  // Log if frame rate was set from environment variable
  SDL_Log(
      "Target frame rate set from environment variable "
      "SDL_TARGET_FRAME_RATE=%f",
      TARGET_FRAME_RATE);

  points.resize(POINT_COUNT);
  for (int i = 0; i < POINT_COUNT; i++) {
    point_speeds.push_back(SDL_randf() * MAX_POINT_SPEED + MIN_POINT_SPEED);

    points[i].resize(TRAIL_LENGTH);

    float initial_x = 0;
    float initial_y = 0;

    initial_x = SDL_randf() * WINDOW_WIDTH;
    initial_y = SDL_randf() * WINDOW_HEIGHT;

    for (int j = 0; j < TRAIL_LENGTH; j++) {
      points[i][j].x = initial_x - j;
      points[i][j].y = initial_y - j;
    }
  }
}

AppContext::~AppContext() {
  // Font is automatically cleaned up by RAII
  TTF_Quit();
}
