#include "graphics.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <spdlog/spdlog.h>

#include <cmath>
#include <vector>

#include "constants.h"
#include "context.h"

Graphics::Graphics(AppContext* context) : context(context) {
  this->context->points.resize(POINT_COUNT);
  for (int i = 0; i < POINT_COUNT; i++) {
    this->context->point_speeds.push_back(SDL_randf() * MAX_POINT_SPEED +
                                          MIN_POINT_SPEED);

    this->context->points[i].resize(TRAIL_LENGTH);

    float initial_x = 0;
    float initial_y = 0;

    initial_x = SDL_randf() * WINDOW_WIDTH;
    initial_y = SDL_randf() * WINDOW_HEIGHT;

    for (int j = 0; j < TRAIL_LENGTH; j++) {
      this->context->points[i][j].x = initial_x - j;
      this->context->points[i][j].y = initial_y - j;
    }
  }
}

void Graphics::renderPoints(float deltaTime) {
  int current_width, current_height;
  SDL_GetWindowSize(context->window, &current_width, &current_height);

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

void Graphics::renderText(const std::string& text, int x, int y,
                          SDL_Color color) {
  // Check if font is loaded
  if (!this->context->font.get()) {
    SPDLOG_WARN("Font not loaded, skipping text rendering");
    return;
  }

  SDL_Surface* text_surface = TTF_RenderText_Blended(
      this->context->font.get(), text.c_str(), text.length(), color);
  if (!text_surface) {
    SPDLOG_ERROR("Couldn't render text: %s", SDL_GetError());
    return;
  }

  SDL_Texture* text_texture =
      SDL_CreateTextureFromSurface(this->context->renderer, text_surface);
  if (!text_texture) {
    SPDLOG_ERROR("Couldn't create texture from surface: %s", SDL_GetError());
    SDL_DestroySurface(text_surface);
    return;
  }

  SDL_FRect dest_rect = {(float)x, (float)y, (float)text_surface->w,
                         (float)text_surface->h};
  SDL_RenderTexture(this->context->renderer, text_texture, nullptr, &dest_rect);

  SDL_DestroySurface(text_surface);
  SDL_DestroyTexture(text_texture);
}
