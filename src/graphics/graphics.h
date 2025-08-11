#pragma once

#include <string>

#include "context.h"

class Graphics {
 private:
  AppContext* context;

 public:
  Graphics(AppContext* context);
  ~Graphics() = default;

  void renderPoints(float deltaTime);
  void renderText(const std::string& text, int x, int y,
                  SDL_Color color = {255, 255, 255, 255});
};
