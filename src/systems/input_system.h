#pragma once

#include <SDL3/SDL.h>

#include <unordered_map>

#include "core/app_state.h"

/**
 * @brief Handles all input events and provides input state management
 *
 * This system is responsible for:
 * - Processing SDL events
 * - Managing input state (key states, mouse position, etc.)
 * - Providing clean input interfaces for other systems
 * - Handling key bindings and shortcuts
 * - Managing entity drag operations
 */
class InputSystem {
 private:
  AppState* appState;

  // Input state tracking
  std::unordered_map<SDL_Keycode, bool> keyStates;
  std::unordered_map<SDL_Keycode, bool>
      previousKeyStates;  // For "just pressed" detection
  SDL_FPoint mousePosition;
  bool mouseButtons[3] = {false, false, false};  // Left, Middle, Right
  bool previousMouseButtons[3] = {false, false,
                                  false};  // For "just pressed" detection

  // Drag state tracking
  bool isDragging = false;
  Entity* draggedEntity = nullptr;
  SDL_FPoint dragOffset;  // Offset from entity center to mouse position
  SDL_FPoint dragStartPosition;
  bool quitRequested = false;

  // Key bindings
  void handleKeyDown(SDL_Keycode key);
  void handleKeyUp(SDL_Keycode key);
  void handleMouseMove(int x, int y);
  void handleMouseButton(int button, bool pressed);

  // Drag functionality
  void startDrag(Entity* entity);
  void updateDrag();
  void endDrag();
  Entity* findEntityUnderMouse();

  // Helper method to check if an entity can be dragged
  bool canEntityBeDragged(Entity* entity) const;

 public:
  InputSystem(AppState* appState) : appState(appState) {}

  /**
   * @brief Process all pending SDL events
   */
  void processEvents();

  /**
   * @brief Check if a key is currently pressed
   */
  bool isKeyPressed(SDL_Keycode key) const;

  /**
   * @brief Check if a key was just pressed this frame
   */
  bool isKeyJustPressed(SDL_Keycode key) const;

  /**
   * @brief Check if a mouse button was just pressed this frame
   */
  bool isMouseButtonJustPressed(int button) const;

  /**
   * @brief Get current mouse position
   */
  SDL_FPoint getMousePosition() const { return mousePosition; }

  /**
   * @brief Check if a mouse button is pressed
   */
  bool isMouseButtonPressed(int button) const;

  /**
   * @brief Check if currently dragging an entity
   */
  bool isDraggingEntity() const { return isDragging; }

  /**
   * @brief Get the currently dragged entity (if any)
   */
  Entity* getDraggedEntity() const { return draggedEntity; }

  /**
   * @brief Check if the application should quit
   */
  bool shouldQuit() const { return quitRequested; }

  /**
   * @brief Update input state (called at the end of frame)
   */
  void update();
};
