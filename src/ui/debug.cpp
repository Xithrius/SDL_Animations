#include "debug.h"

#include "entities/circle.h"
#include "entities/line.h"
#include "entities/rectangle.h"
#include "entities/triangle.h"
#include "event_loop.h"
#include "imgui.h"
#include "systems/animation_system.h"
#include "systems/input_system.h"
#include "ui/ui.h"

void DebugUI::debugOptions() {
  ImGui::Checkbox("Debug Frames", &this->debugFrames);
  if (this->debugFrames) {
  }

  ImGui::Separator();
  ImGui::Text("Framerate Controls:");

  float targetFPS = getUI()->getEventLoop()->getTargetFPS();

  if (ImGui::SliderFloat("Target FPS", &targetFPS, 0.0f, 240.0f, "%.0f")) {
    getUI()->getEventLoop()->setTargetFPS(targetFPS);
  }

  ImGui::Text("Current FPS: %.1f", this->getAppState()->io->Framerate);

  if (ImGui::Button("30 FPS")) {
    targetFPS = 30.0f;
    getUI()->getEventLoop()->setTargetFPS(targetFPS);
  }
  ImGui::SameLine();
  if (ImGui::Button("60 FPS")) {
    targetFPS = 60.0f;
    getUI()->getEventLoop()->setTargetFPS(targetFPS);
  }
  ImGui::SameLine();
  if (ImGui::Button("120 FPS")) {
    targetFPS = 120.0f;
    getUI()->getEventLoop()->setTargetFPS(targetFPS);
  }
  ImGui::SameLine();
  if (ImGui::Button("Unlimited")) {
    targetFPS = 0.0f;
    getUI()->getEventLoop()->setTargetFPS(targetFPS);
  }

  // Line rotation controls
  ImGui::Separator();
  ImGui::Text("Line Rotation Controls:");

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

void DebugUI::render() {
  if (!this->visible) return;

  ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);
  ImGui::Begin("Settings", &this->visible);

  this->debugOptions();

  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
              1000.0f / getAppState()->io->Framerate,
              getAppState()->io->Framerate);

  ImGui::Separator();
  ImGui::Text("Entity System:");

  ImGui::Text("Entity Count: %zu",
              getAppState()->entityManager.getEntityCount());

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
  ImGui::Separator();
  ImGui::Text("Input States:");
  ImGui::Text("F3 Pressed: %s",
              getAppState()->inputSystem->isKeyPressed(SDLK_F3) ? "YES" : "NO");
  ImGui::Text(
      "F3 Just Pressed: %s",
      getAppState()->inputSystem->isKeyJustPressed(SDLK_F3) ? "YES" : "NO");
  ImGui::Text(
      "Left Mouse: %s",
      getAppState()->inputSystem->isMouseButtonPressed(1) ? "YES" : "NO");
  ImGui::Text(
      "Left Mouse Just Pressed: %s",
      getAppState()->inputSystem->isMouseButtonJustPressed(1) ? "YES" : "NO");

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

  if (ImGui::Button("Clear All Entities")) {
    getAppState()->entityManager.clear();
  }

  ImGui::End();
}
