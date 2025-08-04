
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
      font(nullptr),
      last_frame_time(0),
      target_frame_rate(TARGET_FRAME_RATE),
      target_frame_time(0.0f) {
  if (!SDL_CreateWindowAndRenderer(APPLICATION_TITLE.c_str(), WINDOW_WIDTH,
                                   WINDOW_HEIGHT, WINDOW_FLAGS, &window,
                                   &renderer)) {
    SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
    throw std::runtime_error("Failed to create window/renderer");
  }

  // Initialize SDL_ttf
  if (TTF_Init() < 0) {
    SDL_Log("Couldn't initialize SDL_ttf: %s", SDL_GetError());
    throw std::runtime_error("Failed to initialize SDL_ttf");
  }

  // Load a default font (you can replace this with your own font file)
  font = TTF_OpenFont("../assets/fonts/SpaceMono-Regular.ttf", 24);
  if (!font) {
    SDL_Log("Couldn't load font: %s", SDL_GetError());
    // Don't throw here, just log the error - text rendering will be disabled
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

AppContext::~AppContext() {
  if (font) {
    TTF_CloseFont(font);
  }
  TTF_Quit();
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

void AppContext::renderText(const std::string& text, int x, int y,
                            SDL_Color color) {
  if (!font) {
    return;  // Font not loaded, skip text rendering
  }

  SDL_Surface* text_surface =
      TTF_RenderText_Solid(font, text.c_str(), text.length(), color);
  if (!text_surface) {
    SDL_Log("Couldn't render text: %s", SDL_GetError());
    return;
  }

  SDL_Texture* text_texture =
      SDL_CreateTextureFromSurface(renderer, text_surface);
  if (!text_texture) {
    SDL_Log("Couldn't create texture from surface: %s", SDL_GetError());
    SDL_DestroySurface(text_surface);
    return;
  }

  SDL_FRect dest_rect = {(float)x, (float)y, (float)text_surface->w,
                         (float)text_surface->h};
  SDL_RenderTexture(renderer, text_texture, nullptr, &dest_rect);

  SDL_DestroySurface(text_surface);
  SDL_DestroyTexture(text_texture);
}
