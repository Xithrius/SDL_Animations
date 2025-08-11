#include <SDL3/SDL.h>
#include <spdlog/spdlog.h>

#include <memory>

#include "constants.h"
#include "event_loop.h"

int main() {
  spdlog::set_pattern("[%D %H:%M:%S %z] [%^%l%$] %v");

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SPDLOG_ERROR("Couldn't initialize SDL: {}", SDL_GetError());
    return 1;
  }

  SDL_SetAppMetadata(APPLICATION_TITLE.c_str(), VERSION_STRING.c_str(),
                     APPLICATION_IDENTIFIER.c_str());

  std::unique_ptr<EventLoop> event_loop = std::make_unique<EventLoop>();
  event_loop->setTargetFPS(TARGET_FRAME_RATE);

  event_loop->run();

  SDL_Quit();
  return 0;
}
