#include "input_system.h"

#include <spdlog/spdlog.h>

#include "backends/imgui_impl_sdl3.h"

void InputSystem::processEvents() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    // Process ImGui events first
    ImGui_ImplSDL3_ProcessEvent(&event);

    switch (event.type) {
      case SDL_EVENT_QUIT:
        // Handle quit event
        quitRequested = true;
        break;

      case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
        if (event.window.windowID ==
            SDL_GetWindowID(appState->context->window)) {
          // Handle window close
          quitRequested = true;
        }
        break;

      case SDL_EVENT_KEY_DOWN:
        handleKeyDown(event.key.key);
        break;

      case SDL_EVENT_KEY_UP:
        handleKeyUp(event.key.key);
        break;

      case SDL_EVENT_MOUSE_MOTION:
        handleMouseMove(event.motion.x, event.motion.y);
        break;

      case SDL_EVENT_MOUSE_BUTTON_DOWN:
        handleMouseButton(event.button.button, true);
        break;

      case SDL_EVENT_MOUSE_BUTTON_UP:
        handleMouseButton(event.button.button, false);
        break;
    }
  }
}

void InputSystem::handleKeyDown(SDL_Keycode key) {
  keyStates[key] = true;

  // Handle specific key bindings
  switch (key) {
    case SDLK_F3:
      // Toggle debug UI - this should be handled by UI system
      break;
    case SDLK_ESCAPE:
      // Handle escape key
      break;
  }
}

void InputSystem::handleKeyUp(SDL_Keycode key) { keyStates[key] = false; }

void InputSystem::handleMouseMove(int x, int y) {
  mousePosition.x = static_cast<float>(x);
  mousePosition.y = static_cast<float>(y);

  // Update drag if currently dragging
  if (isDragging) {
    updateDrag();
  }
}

void InputSystem::handleMouseButton(int button, bool pressed) {
  if (button >= 1 && button <= 3) {
    mouseButtons[button - 1] = pressed;
  }

  // Handle left mouse button for drag operations
  if (button == SDL_BUTTON_LEFT) {
    if (pressed && !isDragging) {
      // Try to start dragging an entity
      Entity* entityUnderMouse = findEntityUnderMouse();
      if (entityUnderMouse) {
        startDrag(entityUnderMouse);
      }
    } else if (!pressed && isDragging) {
      // End drag operation
      endDrag();
    }
  }
}

Entity* InputSystem::findEntityUnderMouse() {
  // Get all entities and check if mouse is over any of them
  auto entities = appState->entityManager.getAllEntities();

  // Check entities in reverse order (top to bottom) to get the topmost entity
  for (auto it = entities.rbegin(); it != entities.rend(); ++it) {
    Entity* entity = *it;
    if (entity && entity->isActive() && entity->isVisible()) {
      BoundingBox bbox = entity->getBoundingBox();
      if (mousePosition.x >= bbox.minX && mousePosition.x <= bbox.maxX &&
          mousePosition.y >= bbox.minY && mousePosition.y <= bbox.maxY) {
        // Only return entities that can be dragged
        if (canEntityBeDragged(entity)) {
          return entity;
        }
      }
    }
  }
  return nullptr;
}

bool InputSystem::canEntityBeDragged(Entity* entity) const {
  if (!entity) return false;

  // Check if entity implements IInteractive and can be dragged
  if (auto* interactive = entity->asInteractive()) {
    return interactive->canBeDragged();
  }

  // Fallback: check if entity implements IPositionable
  return entity->asPositionable() != nullptr;
}

void InputSystem::startDrag(Entity* entity) {
  if (!entity || !canEntityBeDragged(entity)) return;

  isDragging = true;
  draggedEntity = entity;
  dragStartPosition = mousePosition;

  // Notify the entity that dragging has started
  if (auto* interactive = entity->asInteractive()) {
    interactive->onDragStart();
  }

  // Calculate offset from entity center to mouse position
  BoundingBox bbox = entity->getBoundingBox();
  SDL_FPoint entityCenter = {(bbox.minX + bbox.maxX) / 2.0f,
                             (bbox.minY + bbox.maxY) / 2.0f};

  dragOffset.x = mousePosition.x - entityCenter.x;
  dragOffset.y = mousePosition.y - entityCenter.y;

  spdlog::debug("Started dragging entity: {}", entity->getUUID());
}

void InputSystem::updateDrag() {
  if (!isDragging || !draggedEntity) return;

  // Only update position if entity is positionable
  if (auto* positionable = draggedEntity->asPositionable()) {
    // Calculate new position for the entity
    SDL_FPoint newPosition = {mousePosition.x - dragOffset.x,
                              mousePosition.y - dragOffset.y};

    // Update entity position
    positionable->setPosition(newPosition);
  }
}

void InputSystem::endDrag() {
  if (isDragging && draggedEntity) {
    // Notify the entity that dragging has ended
    if (auto* interactive = draggedEntity->asInteractive()) {
      interactive->onDragEnd();
    }

    spdlog::debug("Ended dragging entity: {}", draggedEntity->getUUID());
  }

  isDragging = false;
  draggedEntity = nullptr;
}

bool InputSystem::isKeyPressed(SDL_Keycode key) const {
  auto it = keyStates.find(key);
  return it != keyStates.end() && it->second;
}

bool InputSystem::isKeyJustPressed(SDL_Keycode key) const {
  auto it = keyStates.find(key);
  auto prevIt = previousKeyStates.find(key);

  // Key is pressed this frame but wasn't pressed last frame
  bool isPressedNow = (it != keyStates.end() && it->second);
  bool wasPressedBefore = (prevIt != previousKeyStates.end() && prevIt->second);

  return isPressedNow && !wasPressedBefore;
}

bool InputSystem::isMouseButtonPressed(int button) const {
  if (button >= 1 && button <= 3) {
    return mouseButtons[button - 1];
  }
  return false;
}

bool InputSystem::isMouseButtonJustPressed(int button) const {
  if (button >= 1 && button <= 3) {
    int index = button - 1;
    return mouseButtons[index] && !previousMouseButtons[index];
  }
  return false;
}

void InputSystem::update() {
  // Update previous key states for "just pressed" detection
  previousKeyStates = keyStates;

  // Update previous mouse button states for "just pressed" detection
  for (int i = 0; i < 3; ++i) {
    previousMouseButtons[i] = mouseButtons[i];
  }
}
