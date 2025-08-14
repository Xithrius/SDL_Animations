#include "context.h"

#include <spdlog/spdlog.h>

#include <stdexcept>

#include "constants.h"

Context::Context() {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    spdlog::error("SDL could not initialize! SDL_Error: {}", SDL_GetError());
    throw std::runtime_error("Failed to initialize SDL");
  }

  if (!TTF_Init()) {
    spdlog::error("SDL_ttf could not initialize! SDL_Error: {}",
                  SDL_GetError());
    throw std::runtime_error("Failed to initialize SDL_ttf");
  }

  if (!SDL_CreateWindowAndRenderer(APPLICATION_TITLE.c_str(),
                                   static_cast<int>(WINDOW_WIDTH),
                                   static_cast<int>(WINDOW_HEIGHT),
                                   SDL_WINDOW_FLAGS, &window, &renderer)) {
    spdlog::error("Couldn't create window/renderer: {}", SDL_GetError());
    throw std::runtime_error("Failed to create window/renderer");
  }
}

Context::~Context() {
  if (renderer) {
    SDL_DestroyRenderer(renderer);
  }
  if (window) {
    SDL_DestroyWindow(window);
  }
  TTF_Quit();
  SDL_Quit();
}
