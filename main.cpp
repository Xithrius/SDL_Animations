#include <SDL3/SDL_log.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <stdexcept>
#include <string>
#include <vector>

const std::string APPLICATION_TITLE = "SDL3 Animations";
const std::string APPLICATION_IDENTIFIER = "com.example.animations";
const std::string VERSION_STRING = "0.1.0";

const float TARGET_FRAME_RATE = 144.0f;
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

class AppContext {
 public:
  SDL_Window *window;
  SDL_Renderer *renderer;
  std::vector<std::vector<SDL_FPoint>> points;
  std::vector<float> point_speeds;
  Uint64 last_frame_time;
  float target_frame_rate;
  float target_frame_time;

  AppContext()
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
};

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

  return SDL_APP_CONTINUE; /* carry on with the program! */
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS; /* end the program, reporting success to the OS. */
  }

  return SDL_APP_CONTINUE; /* carry on with the program! */
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate) {
  AppContext *context = static_cast<AppContext *>(appstate);

  Uint64 current_time = SDL_GetTicks();
  float elapsed_time = (float)(current_time - context->last_frame_time);
  context->last_frame_time = current_time;

  if (elapsed_time < context->target_frame_time) {
    SDL_Delay((Uint32)(context->target_frame_time - elapsed_time));
  }

  float delta_seconds = elapsed_time / 1000.0f;

  // Get current window dimensions
  int current_width, current_height;
  SDL_GetWindowSize(context->window, &current_width, &current_height);

  for (int i = 0; i < context->points.size(); i++) {
    auto &points = context->points[i];
    for (int j = 0; j < points.size(); j++) {
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

    for (int i = 0; i < context->points.size(); i++) {
      column_points.push_back(context->points[i][j]);
    }

    // Render all points at this trail position in one batch
    SDL_RenderPoints(context->renderer, column_points.data(),
                     column_points.size());
  }

  SDL_RenderPresent(context->renderer);

  return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  AppContext *context = static_cast<AppContext *>(appstate);
  delete context;
}
