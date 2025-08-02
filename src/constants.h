#pragma once

#include <SDL3/SDL_video.h>

#include <string>

const std::string APPLICATION_TITLE = "SDL3 Animations";
const std::string APPLICATION_IDENTIFIER = "com.example.animations";
const std::string VERSION_STRING = "0.1.0";

const float TARGET_FRAME_RATE = 60.0f;
const float WINDOW_WIDTH = 1920.0f;
const float WINDOW_HEIGHT = 1080.0f;

const int WINDOW_FLAGS = SDL_WINDOW_RESIZABLE;

const int WINDOW_X_PADDING = WINDOW_WIDTH / 4;
const int WINDOW_Y_PADDING = WINDOW_HEIGHT / 4;

const int WINDOW_X_CONTRAINTS = WINDOW_WIDTH - WINDOW_X_PADDING * 2;
const int WINDOW_Y_CONTRAINTS = WINDOW_HEIGHT - WINDOW_Y_PADDING * 2;

const bool RESTRICT_POINT_SPAWN = false;

const int POINT_COUNT = 1000;
const int TRAIL_LENGTH = 20;

const float MAX_POINT_SPEED = 100.0f;
const float MIN_POINT_SPEED = 10.0f;
