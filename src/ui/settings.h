#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <spdlog/spdlog.h>

#include "core/app_state.h"
#include "ui/ui_component.h"

class SettingsUI : public UIComponent {
 private:
  bool visible = false;
  bool entitySystemDemo = false;
  bool debugFrames = false;

  void MainOptions();

 public:
  SettingsUI(AppState* appState, UI* ui) : UIComponent(appState, ui) {}

  void render() override;

  void toggle() { visible = !visible; }

  bool isVisible() const { return visible; }

  bool isDebugFramesEnabled() const { return debugFrames; }
};
