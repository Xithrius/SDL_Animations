#pragma once

#include <memory>
#include <vector>

#include "core/app_state.h"

/**
 * @brief Handles entity animations and movement
 *
 * This system is responsible for:
 * - Managing entity animations
 * - Handling movement and interpolation
 * - Coordinating with the entity manager
 * - Providing animation utilities
 */
class AnimationSystem {
 private:
  AppState* appState;

 public:
  AnimationSystem(AppState* appState) : appState(appState) {}

  /**
   * @brief Update all animations for this frame
   */
  void update(float deltaTime);

  /**
   * @brief Add an animation to the system
   */
  void addAnimation(std::shared_ptr<class Animation> animation);

  /**
   * @brief Remove an animation from the system
   */
  void removeAnimation(std::shared_ptr<class Animation> animation);

  /**
   * @brief Clear all animations
   */
  void clear();
};

/**
 * @brief Base class for animations
 */
class Animation {
 protected:
  float duration;
  float currentTime;
  bool finished = false;

 public:
  Animation(float duration) : duration(duration), currentTime(0.0f) {}

  virtual ~Animation() = default;

  /**
   * @brief Update the animation
   */
  virtual void update(float deltaTime) = 0;

  /**
   * @brief Check if animation is finished
   */
  bool isFinished() const { return finished; }

  /**
   * @brief Reset the animation
   */
  virtual void reset() {
    currentTime = 0.0f;
    finished = false;
  }
};
