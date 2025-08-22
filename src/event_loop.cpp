#include "event_loop.h"

#include <spdlog/spdlog.h>

#include "systems/animation_system.h"
#include "systems/input_system.h"

EventLoop::EventLoop() {
  try {
    this->context = std::make_unique<Context>();
    this->appState = std::make_unique<AppState>(this->context.get());
    this->ui = std::make_unique<UI>(this->appState.get(), this);
  } catch (const std::exception& e) {
    SPDLOG_ERROR("EventLoop construction failed: %s", e.what());
    throw;
  }
}

/**
 * @brief Runs the event loop.
 */
void EventLoop::run() {
  SPDLOG_INFO("Started event loop");

  Uint64 freq = SDL_GetPerformanceFrequency();
  Uint64 last = SDL_GetPerformanceCounter();

  while (this->running) {
    Uint64 now = SDL_GetPerformanceCounter();
    double deltaTime = (double)(now - last) / freq;
    last = now;

    this->HandleInputEvents();

    this->accumulator += deltaTime;
    while (this->accumulator >= FIXED_TIMESTEP) {
      this->updateEvents(FIXED_TIMESTEP);
      this->accumulator -= FIXED_TIMESTEP;
    }

    this->updateFPS(deltaTime);

    this->render();

    // Calculate target frame time each frame to respond to FPS changes
    double targetFrameTime =
        (this->targetFPS > 0.0f) ? (1.0 / this->targetFPS) : 0.0;

    if (targetFrameTime > 0.0) {
      double elapsed = (double)(SDL_GetPerformanceCounter() - now) / freq;
      if (elapsed < targetFrameTime) {
        double sleepTime = targetFrameTime - elapsed;
        if (sleepTime > 0.001) {  // Only sleep if we have more than 1ms to wait
          SDL_Delay((Uint32)(sleepTime * 1000.0 + 0.5));
        }
      }
    }
  }
}

/**
 * @brief Handles all input events (keyboard, mouse, etc.).
 */
void EventLoop::HandleInputEvents() {
  // Use the InputSystem to process all events
  this->appState->inputSystem->processEvents();

  // Check for quit events
  if (this->appState->inputSystem->shouldQuit()) {
    this->running = false;
    return;
  }

  // Handle application-level events that the InputSystem doesn't handle
  if (this->appState->inputSystem->isKeyJustPressed(SDLK_F1)) {
    this->ui->settings.toggle();
  }

  if (this->appState->inputSystem->isKeyJustPressed(SDLK_F3)) {
    this->ui->debug.toggle();
  }
}

/**
 * @brief All event update logic goes here, nothing should be rendered.
 *
 * @param deltaTime The time since the last update.
 */
void EventLoop::updateEvents(float deltaTime) {
  this->appState->inputSystem->update();
  this->appState->animationSystem->update(deltaTime);
  this->appState->entityManager.update(deltaTime);
}

void EventLoop::updateFPS(float deltaTime) {
  this->frameCount++;
  this->fpsUpdateTimer += deltaTime;

  if (this->fpsUpdateTimer >= 1.0f) {
    this->fps = static_cast<float>(this->frameCount) / this->fpsUpdateTimer;
    this->frameCount = 0;
    this->fpsUpdateTimer = 0.0f;
  }
}

void EventLoop::render() {
  // Clear the renderer at the start of each frame
  SDL_SetRenderScale(this->appState->context->renderer,
                     this->appState->io->DisplayFramebufferScale.x,
                     this->appState->io->DisplayFramebufferScale.y);
  SDL_SetRenderDrawColorFloat(this->appState->context->renderer, 0.0f, 0.0f,
                              0.0f, 1.0f);
  SDL_RenderClear(this->appState->context->renderer);

  // Render entities
  this->appState->entityManager.render(this->appState->context->renderer);

  if (this->ui->debug.isDebugFramesEnabled()) {
    this->renderDebugFrames();
  }

  this->ui->render();

  SDL_RenderPresent(this->appState->context->renderer);
}

void EventLoop::renderDebugFrames() {
  // Set green color for debug frames
  SDL_SetRenderDrawColor(this->appState->context->renderer, 0, 255, 0, 255);

  // Get all entities and draw debug frames around them
  auto entities = this->appState->entityManager.getAllEntities();

  for (auto* entity : entities) {
    if (entity->isVisible()) {
      BoundingBox bbox = entity->getBoundingBox();

      // Draw rectangle from minX,minY to maxX,maxY
      SDL_FRect debugRect = {bbox.minX, bbox.minY, bbox.maxX - bbox.minX,
                             bbox.maxY - bbox.minY};

      // Draw the debug frame rectangle
      SDL_RenderRect(this->appState->context->renderer, &debugRect);

      if (this->ui->debug.isDebugFramesTextEnabled()) {
        renderDebugInfo(entity);
      }
    }
  }
}

void EventLoop::renderDebugInfo(Entity* entity) {
  BoundingBox bbox = entity->getBoundingBox();
  int textX = static_cast<int>(bbox.minX);
  int textY = static_cast<int>(bbox.minY) - 100;

  std::vector<std::string> debugText = getDebugText(entity);

  SDL_Color debugTextColor = {255, 255, 255, 255};  // White

  for (const auto& line : debugText) {
    this->appState->renderer.renderText(line, textX, textY, debugTextColor);
    textY += 20;
  }
}

std::vector<std::string> EventLoop::getDebugText(Entity* entity) {
  BoundingBox bbox = entity->getBoundingBox();
  std::vector<std::string> debugText;

  debugText.push_back("UUID: " + entity->getUUID());
  debugText.push_back("Pos: (" + std::to_string(static_cast<int>(bbox.minX)) +
                      ", " + std::to_string(static_cast<int>(bbox.minY)) + ")");
  debugText.push_back("Type: " + EntityTypeRegistry::getInstance().getTypeName(
                                     entity->getEntityType()));
  debugText.push_back(
      "Size: " + std::to_string(static_cast<int>(bbox.maxX - bbox.minX)) + "x" +
      std::to_string(static_cast<int>(bbox.maxY - bbox.minY)));

  return debugText;
}
