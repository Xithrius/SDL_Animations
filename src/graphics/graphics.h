#pragma once

#include <string>

#include "context.h"
#include "imgui.h"

class Graphics {
 private:
  AppContext* context;

  ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);

 public:
  Graphics(AppContext* context);
  ~Graphics() = default;

  void updatePoints(float deltaTime);

  void renderText(const std::string& text, int x, int y,
                  SDL_Color color = {255, 255, 255, 255});

  void render(float fps, float tickRate);

  void renderUI();
};
