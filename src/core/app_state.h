#pragma once

#include <SDL3/SDL.h>

#include <memory>
#include <vector>

#include "context.h"
#include "entities/entity.h"
#include "graphics/fonts.h"
#include "graphics/renderer.h"
#include "imgui.h"

class InputSystem;
class AnimationSystem;

class AppState {
 public:
  Context* context;
  FontRef font;
  Renderer renderer;

  std::vector<std::vector<SDL_FPoint>> points;
  std::vector<float> point_speeds;
  float point_speed_multiplier = 1.0f;

  float target_frame_rate;
  ImGuiIO* io;

  EntityManager entityManager;
  std::unique_ptr<InputSystem> inputSystem;
  std::unique_ptr<AnimationSystem> animationSystem;

  AppState(Context* context);
  ~AppState();
};
