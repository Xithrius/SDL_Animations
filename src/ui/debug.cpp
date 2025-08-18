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

void DebugUI::renderDebugControls() {
  ImGui::Checkbox("Debug Frames", &this->debugFrames);

  ImGui::Spacing();
  ImGui::SeparatorText("Line Rotation Controls");

  static float rotationSpeed = 1.0f;
  if (ImGui::SliderFloat("Rotation Speed", &rotationSpeed, 0.0f, 5.0f)) {
    // Update rotation speed for all line entities
    auto lines = getAppState()->entityManager.getEntitiesOfType<LineEntity>();
    for (auto* line : lines) {
      line->setRotationSpeed(rotationSpeed);
    }
  }

  if (ImGui::Button("Create Spinning Line")) {
    int windowWidth, windowHeight;
    SDL_GetWindowSize(getAppState()->context->window, &windowWidth,
                      &windowHeight);

    // Create a line at the center of the screen
    SDL_FPoint center = {static_cast<float>(windowWidth) / 2.0f,
                         static_cast<float>(windowHeight) / 2.0f};
    SDL_FPoint start = {center.x - 50.0f, center.y};
    SDL_FPoint end = {center.x + 50.0f, center.y};

    auto* line =
        getAppState()->entityManager.createEntity<LineEntity>(start, end);
    line->setRotationSpeed(rotationSpeed);
    line->setThickness(3.0f);

    // Set gradient properties for visual appeal
    LineEntity::GradientProperties gradientProps;
    gradientProps.enabled = true;
    gradientProps.startColor = {255, 0, 0, 255};  // Red
    gradientProps.endColor = {0, 0, 255, 255};    // Blue
    line->setGradientProperties(gradientProps);
  }
}

void DebugUI::renderDebugInformation() {
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
  // Show drag state information
  if (getAppState()->inputSystem->isDraggingEntity()) {
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Dragging: YES");
    Entity* draggedEntity = getAppState()->inputSystem->getDraggedEntity();
    if (draggedEntity) {
      ImGui::Text("Dragged Entity: %s", draggedEntity->getUUID().c_str());
    }
  } else {
    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Dragging: NO");
  }

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

  if (ImGui::Button("Create Draggable Test Entities")) {
    int windowWidth, windowHeight;
    SDL_GetWindowSize(getAppState()->context->window, &windowWidth,
                      &windowHeight);

    // Create a red circle for dragging
    auto* circle = getAppState()->entityManager.createEntity<CircleEntity>(
        SDL_FPoint{windowWidth / 2.0f, windowHeight / 2.0f}, 30.0f);
    circle->setColor({255, 0, 0, 255});  // Red
    circle->setFilled(true);

    // Create a blue rectangle for dragging
    auto* rect = getAppState()->entityManager.createEntity<RectangleEntity>(
        SDL_FRect{windowWidth / 2.0f - 40.0f, windowHeight / 2.0f - 40.0f,
                  80.0f, 80.0f});
    rect->setColor({0, 0, 255, 255});  // Blue
    rect->setFilled(true);

    // Create a green triangle for dragging
    auto* triangle = getAppState()->entityManager.createEntity<TriangleEntity>(
        SDL_FPoint{windowWidth / 2.0f - 50.0f, windowHeight / 2.0f + 50.0f},
        SDL_FPoint{windowWidth / 2.0f + 50.0f, windowHeight / 2.0f + 50.0f},
        SDL_FPoint{windowWidth / 2.0f, windowHeight / 2.0f + 100.0f});
    triangle->setColor({0, 255, 0, 255});  // Green
    triangle->setFilled(true);
  }

  if (ImGui::Button("Create Non-Draggable Text")) {
    int windowWidth, windowHeight;
    SDL_GetWindowSize(getAppState()->context->window, &windowWidth,
                      &windowHeight);
  }

  if (ImGui::Button("Create Rotating Line")) {
    int windowWidth, windowHeight;
    SDL_GetWindowSize(getAppState()->context->window, &windowWidth,
                      &windowHeight);

    // Create a line that rotates and can be dragged
    SDL_FPoint center = {static_cast<float>(windowWidth) / 2.0f,
                         static_cast<float>(windowHeight) / 2.0f};
    SDL_FPoint start = {center.x - 50.0f, center.y};
    SDL_FPoint end = {center.x + 50.0f, center.y};

    auto* line =
        getAppState()->entityManager.createEntity<LineEntity>(start, end);
    line->setRotationSpeed(2.0f);  // Rotate at 2 radians per second
    line->setThickness(3.0f);
    line->setColor({255, 0, 255, 255});  // Magenta
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

    // Create a circle that cannot be dragged
    auto* circle = getAppState()->entityManager.createEntity<CircleEntity>(
        SDL_FPoint{windowWidth / 2.0f + 100.0f, windowHeight / 2.0f}, 25.0f);
    circle->setColor({128, 128, 128, 255});  // Gray
    circle->setFilled(true);
    circle->setDraggable(false);  // Disable dragging
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

  if (ImGui::Button("Create 4x4 Isometric Grid of Cubes")) {
    int windowWidth, windowHeight;
    SDL_GetWindowSize(getAppState()->context->window, &windowWidth,
                      &windowHeight);

    SDL_FPoint position = {windowWidth / 2.0f, windowHeight / 2.0f};

    for (int i = 0; i < 4; i++) {
      auto cube =
          getAppState()->entityManager.createEntity<IsometricCubeEntity>(
              getAppState());
      cube->setPosition(position);
      position = cube->getFrontRight();
    }
  }
}

void DebugUI::renderEntityManagement() {
  if (ImGui::Button("Regenerate Waypoint Positions")) {
    // Find all waypoint entities and regenerate their random positions
    auto entities = getAppState()->entityManager.getAllEntities();
    for (auto* entity : entities) {
      if (auto* waypoint = dynamic_cast<WaypointEntity*>(entity)) {
        waypoint->regenerateRandomPosition();
      }
    }
  }

  if (ImGui::Button("Change Waypoint Speed")) {
    // Change movement speed for all waypoints
    auto entities = getAppState()->entityManager.getAllEntities();
    for (auto* entity : entities) {
      if (auto* waypoint = dynamic_cast<WaypointEntity*>(entity)) {
        // Cycle through different speeds: 0.5, 1.0, 2.0, 5.0
        float currentSpeed = waypoint->getMovementSpeed();
        if (currentSpeed < 0.5f)
          waypoint->setMovementSpeed(0.5f);
        else if (currentSpeed < 1.0f)
          waypoint->setMovementSpeed(1.0f);
        else if (currentSpeed < 2.0f)
          waypoint->setMovementSpeed(2.0f);
        else if (currentSpeed < 5.0f)
          waypoint->setMovementSpeed(5.0f);
        else
          waypoint->setMovementSpeed(0.5f);
      }
    }
  }

  if (ImGui::Button("Print Waypoint Positions")) {
    // Print current positions of all waypoints
    auto entities = getAppState()->entityManager.getAllEntities();
    int waypointCount = 0;
    for (auto* entity : entities) {
      if (auto* waypoint = dynamic_cast<WaypointEntity*>(entity)) {
        waypointCount++;
        spdlog::info("Waypoint {}: current=({:.1f}, {:.1f})", waypointCount,
                     waypoint->getCurrentPosition().x,
                     waypoint->getCurrentPosition().y);
      }
    }
    if (waypointCount == 0) {
      spdlog::info("No waypoints found");
    }
  }

  if (ImGui::Button("Clear All Entities")) {
    getAppState()->entityManager.clear();
  }
}

void DebugUI::render() {
  if (!this->visible) return;

  ImGui::SetNextWindowSize(ImVec2(400, 500), ImGuiCond_FirstUseEver);
  ImGui::Begin("Debug Panel", &this->visible);

  // Render all debug UI sections
  this->renderDebugControls();
  this->renderDebugInformation();
  this->renderInputStates();
  this->renderEntityCreation();
  this->renderEntityManagement();

  ImGui::End();
}
