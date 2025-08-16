#pragma once

#include <spdlog/spdlog.h>

#include <memory>
#include <vector>

#include "core/app_state.h"

// Forward declarations
class Entity;
class Animation;

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
  std::vector<std::shared_ptr<Animation>> animations;
  std::vector<std::shared_ptr<Animation>> animationsToRemove;

 public:
  AnimationSystem(AppState* appState) : appState(appState) {}

  /**
   * @brief Update all animations for this frame
   */
  void update(float deltaTime);

  /**
   * @brief Add an animation to the system
   */
  void addAnimation(std::shared_ptr<Animation> animation);

  /**
   * @brief Remove an animation from the system
   */
  void removeAnimation(std::shared_ptr<Animation> animation);

  /**
   * @brief Clear all animations
   */
  void clear();

  /**
   * @brief Get all animations for a specific entity
   */
  std::vector<std::shared_ptr<Animation>> getAnimationsForEntity(
      Entity* entity);
};

/**
 * @brief Base class for animations
 */
class Animation {
 protected:
  float duration;
  float currentTime;
  bool finished = false;
  bool looping = false;
  Entity* targetEntity = nullptr;

 public:
  Animation(float duration, Entity* targetEntity = nullptr)
      : duration(duration), currentTime(0.0f), targetEntity(targetEntity) {}

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
   * @brief Check if animation is looping
   */
  bool isLooping() const { return looping; }

  /**
   * @brief Set looping state
   */
  void setLooping(bool loop) { looping = loop; }

  /**
   * @brief Get the target entity
   */
  Entity* getTargetEntity() const { return targetEntity; }

  /**
   * @brief Set the target entity
   */
  void setTargetEntity(Entity* entity) { targetEntity = entity; }

  /**
   * @brief Reset the animation
   */
  virtual void reset() {
    currentTime = 0.0f;
    finished = false;
  }

  /**
   * @brief Get animation progress (0.0 to 1.0)
   */
  float getProgress() const {
    return duration > 0.0f ? std::min(currentTime / duration, 1.0f) : 0.0f;
  }
};

/**
 * @brief Animation for cycling through gradient colors
 */
class GradientAnimation : public Animation {
 private:
  std::vector<SDL_Color> colors;
  size_t currentColorIndex = 0;
  size_t nextColorIndex = 1;
  float colorTransitionTime = 0.0f;
  float colorTransitionDuration = 1.0f;

 public:
  GradientAnimation(float duration, Entity* targetEntity,
                    const std::vector<SDL_Color>& colors)
      : Animation(duration, targetEntity), colors(colors) {
    if (colors.size() < 2) {
      spdlog::warn("GradientAnimation requires at least 2 colors");
    }
  }

  void update(float deltaTime) override {
    if (colors.size() < 2) return;

    currentTime += deltaTime;
    colorTransitionTime += deltaTime;

    // Check if we should transition to next color
    if (colorTransitionTime >= colorTransitionDuration) {
      currentColorIndex = nextColorIndex;
      nextColorIndex = (nextColorIndex + 1) % colors.size();
      colorTransitionTime = 0.0f;
    }

    // Check if animation is finished
    if (currentTime >= duration && !looping) {
      finished = true;
    }
  }

  /**
   * @brief Get the current interpolated color
   */
  SDL_Color getCurrentColor() const {
    if (colors.empty()) return {255, 255, 255, 255};
    if (colors.size() == 1) return colors[0];

    float t = colorTransitionTime / colorTransitionDuration;
    const SDL_Color& color1 = colors[currentColorIndex];
    const SDL_Color& color2 = colors[nextColorIndex];

    // Interpolate between colors
    SDL_Color result;
    result.r = static_cast<Uint8>(color1.r + (color2.r - color1.r) * t);
    result.g = static_cast<Uint8>(color1.g + (color2.g - color1.g) * t);
    result.b = static_cast<Uint8>(color1.b + (color2.b - color1.b) * t);
    result.a = static_cast<Uint8>(color1.a + (color2.a - color1.a) * t);

    return result;
  }

  /**
   * @brief Get the next color in the sequence
   */
  SDL_Color getNextColor() const {
    if (colors.empty()) return {255, 255, 255, 255};
    return colors[nextColorIndex];
  }

  /**
   * @brief Set the color transition duration
   */
  void setColorTransitionDuration(float duration) {
    colorTransitionDuration = duration;
  }

  /**
   * @brief Get the color transition duration
   */
  float getColorTransitionDuration() const { return colorTransitionDuration; }
};
