#include <SDL3/SDL.h>
#include <SDL3/SDL_log.h>

#include <memory>

#include "constants.h"
#include "event_loop.h"

int main() {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
    return 1;
  }

  SDL_SetAppMetadata(APPLICATION_TITLE.c_str(), VERSION_STRING.c_str(),
                     APPLICATION_IDENTIFIER.c_str());

  std::unique_ptr<EventLoop> event_loop = std::make_unique<EventLoop>();
  event_loop->setTargetFPS(TARGET_FRAME_RATE);

  SDL_Log("Starting event loop...");
  event_loop->run();

  SDL_Quit();
  return 0;
}
