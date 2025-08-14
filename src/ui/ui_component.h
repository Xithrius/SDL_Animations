#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <spdlog/spdlog.h>

#include "core/app_state.h"

// Forward declaration to avoid circular dependency
class UI;

/**
 * @brief Base class for UI components that need access to both AppState and UI
 *
 * This class provides a common interface for UI components that need to:
 * - Access application state (AppState)
 * - Call methods on the parent UI class
 * - Render ImGui elements
 */
class UIComponent {
 protected:
  AppState* appState;
  UI* ui;

 public:
  UIComponent(AppState* appState, UI* ui) : appState(appState), ui(ui) {}

  virtual ~UIComponent() = default;

  /**
   * @brief Render the UI component
   *
   * Override this method to implement the component's rendering logic
   */
  virtual void render() = 0;

  /**
   * @brief Get access to the application state
   */
  AppState* getAppState() const { return appState; }

  /**
   * @brief Get access to the parent UI
   */
  UI* getUI() const { return ui; }
};
