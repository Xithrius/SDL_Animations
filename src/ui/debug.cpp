#include "debug.h"

#include "SDL3/SDL_rect.h"
#include "entities/circle.h"
#include "entities/isometric_cube.h"
#include "entities/line.h"
#include "entities/rectangle.h"
#include "entities/triangle.h"
#include "entities/waypoint.h"
#include "imgui.h"
#include "systems/animation_system.h"
#include "systems/input_system.h"

void DebugUI::renderDebugFrameControls() {
  ImGui::Checkbox("Debug Frames", &this->debugFrames);
  ImGui::BeginDisabled(!this->debugFrames);
  ImGui::Checkbox("Debug Text", &this->debugFramesText);
  ImGui::EndDisabled();
}

void DebugUI::renderDebugFramerateInformation() {
  ImGui::Spacing();
  ImGui::SeparatorText("Debug Information");
  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
              1000.0f / getAppState()->io->Framerate,
              getAppState()->io->Framerate);

  ImGui::Spacing();
  ImGui::SeparatorText("Entity System");
  ImGui::Text("Entity Count: %zu",
              getAppState()->entityManager.getEntityCount());
}

void DebugUI::renderInputStates() {
  bool draggingEntity = getAppState()->inputSystem->isDraggingEntity();
  Entity* draggedEntity = getAppState()->inputSystem->getDraggedEntity();
  ImVec4 draggingColor = draggingEntity ? ImVec4(1.0f, 1.0f, 0.0f, 1.0f)
                                        : ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
  ImGui::TextColored(
      draggingColor, "Dragged Entity: %s",
      draggingEntity ? draggedEntity->getUUID().c_str() : "None");

  ImGui::Text("Mouse Position: (%.1f, %.1f)",
              getAppState()->inputSystem->getMousePosition().x,
              getAppState()->inputSystem->getMousePosition().y);

  // Show key states
  ImGui::Spacing();
  ImGui::SeparatorText("Input States");
  ImGui::Text(
      "Left Mouse: %s",
      getAppState()->inputSystem->isMouseButtonPressed(1) ? "YES" : "NO");
}

void DebugUI::renderEntityCreation() {
  ImGui::Spacing();
  ImGui::SeparatorText("Entity Creation");

  if (ImGui::Button("Add Random Line")) {
    int windowWidth, windowHeight;
    SDL_GetWindowSize(getAppState()->context->window, &windowWidth,
                      &windowHeight);

    auto* line = getAppState()->entityManager.createEntity<LineEntity>(
        SDL_FPoint{SDL_randf() * windowWidth, SDL_randf() * windowHeight},
        SDL_FPoint{SDL_randf() * windowWidth, SDL_randf() * windowHeight});

    LineEntity::GradientProperties gradientProps;
    gradientProps.enabled = true;
    gradientProps.startColor = {static_cast<Uint8>(SDL_randf() * 255),
                                static_cast<Uint8>(SDL_randf() * 255),
                                static_cast<Uint8>(SDL_randf() * 255), 255};
    gradientProps.endColor = {static_cast<Uint8>(SDL_randf() * 255),
                              static_cast<Uint8>(SDL_randf() * 255),
                              static_cast<Uint8>(SDL_randf() * 255), 255};
    line->setGradientProperties(gradientProps);
  }

  if (ImGui::Button("Add Random Circle")) {
    int windowWidth, windowHeight;
    SDL_GetWindowSize(getAppState()->context->window, &windowWidth,
                      &windowHeight);

    auto* circle = getAppState()->entityManager.createEntity<CircleEntity>(
        SDL_FPoint{SDL_randf() * windowWidth, SDL_randf() * windowHeight},
        SDL_randf() * 50.0f + 10.0f);
    circle->setColor({static_cast<Uint8>(SDL_randf() * 255),
                      static_cast<Uint8>(SDL_randf() * 255),
                      static_cast<Uint8>(SDL_randf() * 255), 128});
    circle->setFilled(SDL_randf() > 0.5f);
  }

  if (ImGui::Button("Create Animated Gradient Line")) {
    int windowWidth, windowHeight;
    SDL_GetWindowSize(getAppState()->context->window, &windowWidth,
                      &windowHeight);

    // Create a line with animated gradient
    SDL_FPoint center = {static_cast<float>(windowWidth) / 2.0f,
                         static_cast<float>(windowHeight) / 2.0f - 100.0f};
    SDL_FPoint start = {center.x - 80.0f, center.y};
    SDL_FPoint end = {center.x + 80.0f, center.y};

    auto* line =
        getAppState()->entityManager.createEntity<LineEntity>(start, end);
    line->setThickness(5.0f);
    line->setColor({255, 255, 255, 255});  // Start with white

    // Create gradient animation
    std::vector<SDL_Color> gradientColors = {
        {255, 0, 0, 255},    // Red
        {255, 165, 0, 255},  // Orange
        {255, 255, 0, 255},  // Yellow
        {0, 255, 0, 255},    // Green
        {0, 0, 255, 255},    // Blue
        {128, 0, 128, 255},  // Purple
        {255, 0, 255, 255}   // Magenta
    };

    auto gradientAnim =
        std::make_shared<GradientAnimation>(10.0f,  // 10 second duration
                                            line,   // Target entity
                                            gradientColors);
    gradientAnim->setLooping(true);                  // Loop forever
    gradientAnim->setColorTransitionDuration(1.5f);  // 1.5 seconds per color

    // Add animation to the line
    line->addAnimation(gradientAnim);
  }

  if (ImGui::Button("Create Non-Draggable Circle")) {
    int windowWidth, windowHeight;
    SDL_GetWindowSize(getAppState()->context->window, &windowWidth,
                      &windowHeight);

    auto* circle = getAppState()->entityManager.createEntity<CircleEntity>(
        SDL_FPoint{windowWidth / 2.0f + 100.0f, windowHeight / 2.0f}, 25.0f);
    circle->setColor({128, 128, 128, 255});  // Gray
    circle->setFilled(true);
    circle->setDraggable(false);
  }

  if (ImGui::Button("Create Waypoint")) {
    int windowWidth, windowHeight;
    SDL_GetWindowSize(getAppState()->context->window, &windowWidth,
                      &windowHeight);

    auto* waypoint = getAppState()->entityManager.createEntity<WaypointEntity>(
        getAppState());
    waypoint->setInitialPosition(windowWidth / 2.0f, windowHeight / 2.0f);
  }

  if (ImGui::Button("Create 100 Waypoints")) {
    int windowWidth, windowHeight;
    SDL_GetWindowSize(getAppState()->context->window, &windowWidth,
                      &windowHeight);

    for (int i = 0; i < 100; i++) {
      auto* waypoint =
          getAppState()->entityManager.createEntity<WaypointEntity>(
              getAppState());
      waypoint->setInitialPosition(windowWidth / 2.0f, windowHeight / 2.0f);
    }
  }

  if (ImGui::Button("Create Isometric Cube")) {
    int windowWidth, windowHeight;
    SDL_GetWindowSize(getAppState()->context->window, &windowWidth,
                      &windowHeight);

    auto cube = getAppState()->entityManager.createEntity<IsometricCubeEntity>(
        getAppState());
    cube->setPosition(SDL_FPoint{windowWidth / 2.0f, windowHeight / 2.0f});
  }

  if (ImGui::Button("Create Isometric Grid Wave")) {
    int windowWidth, windowHeight;
    SDL_GetWindowSize(getAppState()->context->window, &windowWidth,
                      &windowHeight);

    SDL_FPoint center = {
        windowWidth / 2.0f, windowHeight / 2.0f,
        // static_cast<float>(windowHeight + (windowHeight / 2.0f)),
    };
    SDL_FPoint rowStart = center;
    int side = 8;
    // int side = 64;

    for (int r = 0; r < side; r++) {
      auto* first =
          getAppState()->entityManager.createEntity<IsometricCubeEntity>(
              getAppState());
      first->setTime(r * 75.0f);
      first->setPosition(rowStart);

      SDL_FPoint colPos = first->getBehindLeft();
      for (int c = 1; c < side; c++) {
        auto* cube =
            getAppState()->entityManager.createEntity<IsometricCubeEntity>(
                getAppState());
        cube->setTime((r + c) * 75.0f);
        cube->setPosition(colPos);
        colPos = cube->getBehindLeft();
      }

      rowStart = first->getBehindRight();
    }
  }

  if (ImGui::Button("Test isometric cube positions")) {
    int windowWidth, windowHeight;
    SDL_GetWindowSize(getAppState()->context->window, &windowWidth,
                      &windowHeight);

    SDL_FPoint center = {windowWidth / 2.0f, windowHeight / 2.0f};

    // Center
    auto center_cube =
        getAppState()->entityManager.createEntity<IsometricCubeEntity>(
            getAppState());
    center_cube->setPosition(center);

    // Left behind
    auto cube = getAppState()->entityManager.createEntity<IsometricCubeEntity>(
        getAppState());
    cube->setPosition(center_cube->getBehindLeft());
    // Right behind
    cube = getAppState()->entityManager.createEntity<IsometricCubeEntity>(
        getAppState());
    cube->setPosition(center_cube->getBehindRight());
    // Left Front
    cube = getAppState()->entityManager.createEntity<IsometricCubeEntity>(
        getAppState());
    cube->setPosition(center_cube->getFrontLeft());
    // Right Front
    cube = getAppState()->entityManager.createEntity<IsometricCubeEntity>(
        getAppState());
    cube->setPosition(center_cube->getFrontRight());
  }
}

void DebugUI::renderEntityManagement() {
  if (ImGui::Button("Clear All Entities")) {
    getAppState()->entityManager.clear();
  }
}

void DebugUI::render() {
  if (!this->visible) return;

  ImGui::SetNextWindowSize(ImVec2(400, 500), ImGuiCond_FirstUseEver);
  ImGui::Begin("Debug Panel", &this->visible);

  this->renderDebugFrameControls();
  this->renderDebugFramerateInformation();
  this->renderInputStates();
  this->renderEntityCreation();
  this->renderEntityManagement();

  ImGui::End();
}
