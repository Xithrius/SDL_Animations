#pragma once

#include <string>

#include "context.h"
#include "imgui.h"

class Graphics {
 private:
  AppContext* context;

  ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);

 public:
  bool show_debug = false;

  Graphics(AppContext* context);
  ~Graphics() = default;


  void renderText(const std::string& text, int x, int y,
                  SDL_Color color = {255, 255, 255, 255});

  void render(float tickRate);

  void renderEntities();

  void renderUI(float tickRate);

  // Entity management helpers
  void createDemoEntities();
  void clearEntities();
};
