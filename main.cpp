#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <new>
#include <stdexcept>
#include <string>

const std::string APPLICATION_TITLE = "SDL3 Animations";
const std::string APPLICATION_IDENTIFIER = "com.example.animations";
const std::string VERSION_STRING = "0.1.0";

const float TARGET_FRAME_RATE = 144.0f;
const float WINDOW_WIDTH = 640.0f;
const float WINDOW_HEIGHT = 480.0f;

class AppContext {
 public:
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_FPoint points[500];
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
                                     WINDOW_HEIGHT, 0, &window, &renderer)) {
      SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
      throw std::runtime_error("Failed to create window/renderer");
    }
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    target_frame_time = 1000.0f / target_frame_rate;

    SDL_Log("Frame rate limited to %.0f FPS (%.2f ms per frame)",
            target_frame_rate, target_frame_time);

    last_frame_time = SDL_GetTicks();

    for (int i = 0; i < 500; i++) {
      points[i].x = (SDL_randf() * 440.0f) + 100.0f;
      points[i].y = (SDL_randf() * 280.0f) + 100.0f;
    }
  }

  ~AppContext() {
    // SDL will clean up window/renderer, but you could add custom cleanup here
    // if needed
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
  SDL_FRect rect;
  Uint64 current_time;
  float elapsed_time;
  AppContext *context = static_cast<AppContext *>(appstate);

  /* Frame rate limiting */
  current_time = SDL_GetTicks();
  elapsed_time = (float)(current_time - context->last_frame_time);
  if (elapsed_time < context->target_frame_time) {
    SDL_Delay((Uint32)(context->target_frame_time - elapsed_time));
  }
  context->last_frame_time = SDL_GetTicks();

  SDL_SetRenderDrawColor(context->renderer, 33, 33, 33, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(context->renderer);
  SDL_SetRenderDrawColor(context->renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
  rect.x = rect.y = 100;
  rect.w = 440;
  rect.h = 280;
  SDL_RenderFillRect(context->renderer, &rect);
  SDL_SetRenderDrawColor(context->renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderPoints(context->renderer, context->points, 500);
  SDL_SetRenderDrawColor(context->renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
  rect.x += 30;
  rect.y += 30;
  rect.w -= 60;
  rect.h -= 60;
  SDL_RenderRect(context->renderer, &rect);
  SDL_SetRenderDrawColor(context->renderer, 255, 255, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderLine(context->renderer, 0, 0, 640, 480);
  SDL_RenderLine(context->renderer, 0, 480, 640, 0);
  SDL_RenderPresent(context->renderer);
  return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  AppContext *context = static_cast<AppContext *>(appstate);
  delete context;
}
