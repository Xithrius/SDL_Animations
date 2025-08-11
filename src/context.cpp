
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

  if (!TTF_Init()) {
    SDL_Log("Couldn't initialize SDL_ttf: %s", SDL_GetError());
    throw std::runtime_error("Failed to initialize SDL_ttf");
  }

  font.loadFont();

  SDL_Log(
      "Target frame rate set from environment variable "
      "SDL_TARGET_FRAME_RATE=%f",
      TARGET_FRAME_RATE);
}

AppContext::~AppContext() {
  if (renderer) {
    SDL_DestroyRenderer(renderer);
    renderer = nullptr;
  }

  if (window) {
    SDL_DestroyWindow(window);
    window = nullptr;
  }
}
