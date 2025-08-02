
#include "context.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>

#include <stdexcept>
#include <string>
#include <vector>

#include "constants.h"

AppContext::AppContext()
    : window(nullptr),
      renderer(nullptr),
      last_frame_time(0),
      target_frame_rate(TARGET_FRAME_RATE),
      target_frame_time(0.0f) {
  if (!SDL_CreateWindowAndRenderer(APPLICATION_TITLE.c_str(), WINDOW_WIDTH,
                                   WINDOW_HEIGHT, WINDOW_FLAGS, &window,
                                   &renderer)) {
    SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
    throw std::runtime_error("Failed to create window/renderer");
  }

  target_frame_time = 1000.0f / target_frame_rate;

  SDL_Log("Frame rate limited to %.0f FPS (%.2f ms per frame)",
          target_frame_rate, target_frame_time);

  last_frame_time = SDL_GetTicks();

  points.resize(POINT_COUNT);
  for (int i = 0; i < POINT_COUNT; i++) {
    point_speeds.push_back(SDL_randf() * MAX_POINT_SPEED + MIN_POINT_SPEED);

    points[i].resize(TRAIL_LENGTH);

    float initial_x = 0;
    float initial_y = 0;

    if (RESTRICT_POINT_SPAWN) {
      initial_x = SDL_randf() * WINDOW_X_CONTRAINTS + WINDOW_X_PADDING;
      initial_y = SDL_randf() * WINDOW_Y_CONTRAINTS + WINDOW_Y_PADDING;
    } else {
      initial_x = SDL_randf() * WINDOW_WIDTH;
      initial_y = SDL_randf() * WINDOW_HEIGHT;
    }

    for (int j = 0; j < TRAIL_LENGTH; j++) {
      points[i][j].x = initial_x - j;
      points[i][j].y = initial_y - j;
    }
  }
}

float AppContext::updateFrameTiming() {
  Uint64 current_time = SDL_GetTicks();
  float elapsed_time = (float)(current_time - this->last_frame_time);
  this->last_frame_time = current_time;

  if (elapsed_time < this->target_frame_time) {
    SDL_Delay((Uint32)(this->target_frame_time - elapsed_time));
  }

  return elapsed_time / 1000.0f;
}
