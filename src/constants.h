#pragma once

#include <SDL3/SDL_video.h>

#include <cstdlib>
#include <string>

const SDL_WindowFlags SDL_WINDOW_FLAGS =
    SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;

const std::string APPLICATION_TITLE = "SDL3 Animations";
const std::string APPLICATION_IDENTIFIER = "com.example.animations";
const std::string VERSION_STRING = "0.1.0";

// Function to get target frame rate from environment variable or use default
inline float getTargetFrameRate() {
  const char* env_frame_rate = std::getenv("TARGET_FRAME_RATE");

  if (env_frame_rate != nullptr) {
    float frame_rate = std::stof(env_frame_rate);
    if (frame_rate > 0.0f) {
      return frame_rate;
    }
  }

  return 60.0f;
}

const float TARGET_FRAME_RATE = getTargetFrameRate();
const float WINDOW_WIDTH = 1920.0f;
const float WINDOW_HEIGHT = 1080.0f;

const int WINDOW_FLAGS = SDL_WINDOW_RESIZABLE;

const int POINT_COUNT = 1000;
const int TRAIL_LENGTH = 20;

const float MAX_POINT_SPEED = 100.0f;
const float MIN_POINT_SPEED = 10.0f;
