#include "line.h"

#include <spdlog/spdlog.h>

#include <cmath>

#include "systems/animation_system.h"

LineEntity::LineEntity(const SDL_FPoint& start, const SDL_FPoint& end)
    : start(start), end(end) {
  // Calculate the origin (center point) of the line
  origin.x = (start.x + end.x) / 2.0f;
  origin.y = (start.y + end.y) / 2.0f;

  // Calculate the length of the line
  float dx = end.x - start.x;
  float dy = end.y - start.y;
  lineLength = sqrtf(dx * dx + dy * dy);
}

void LineEntity::update(float deltaTime) {
  // Update rotation if rotation speed is set
  if (rotationSpeed != 0.0f) {
    angle += rotationSpeed * deltaTime;

    // Keep angle in reasonable range
    while (angle > 2.0f * M_PI) {
      angle -= 2.0f * M_PI;
    }
    while (angle < 0.0f) {
      angle += 2.0f * M_PI;
    }
  }

  // Update animations
  for (auto& animation : animations) {
    if (animation && !animation->isFinished()) {
      animation->update(deltaTime);

      // Handle gradient animations
      if (auto* gradientAnim =
              dynamic_cast<class GradientAnimation*>(animation.get())) {
        setAnimatedColor(gradientAnim->getCurrentColor());
      }
    }
  }

  // Remove finished animations
  animations.erase(
      std::remove_if(animations.begin(), animations.end(),
                     [](const std::shared_ptr<class Animation>& anim) {
                       return anim && anim->isFinished();
                     }),
      animations.end());
}

void LineEntity::render(SDL_Renderer* renderer) {
  if (!visible) return;

  // Use animated color if available, otherwise use base color
  SDL_Color renderColor = animatedColor;
  SDL_SetRenderDrawColor(renderer, renderColor.r, renderColor.g, renderColor.b,
                         renderColor.a);

  // Draw the line
  SDL_RenderLine(renderer, start.x, start.y, end.x, end.y);
}

void LineEntity::addAnimation(std::shared_ptr<class Animation> animation) {
  if (animation) {
    animations.push_back(animation);
  }
}

void LineEntity::removeAnimation(std::shared_ptr<class Animation> animation) {
  auto it = std::find(animations.begin(), animations.end(), animation);
  if (it != animations.end()) {
    animations.erase(it);
  }
}

std::vector<std::shared_ptr<class Animation>> LineEntity::getAnimations()
    const {
  return animations;
}

void LineEntity::clearAnimations() {
  animations.clear();
  animatedColor = color;  // Reset to base color
}

BoundingBox LineEntity::getBoundingBox() const {
  float halfLength = lineLength / 2.0f;
  float halfThickness = thickness / 2.0f;

  float maxExtent = halfLength + halfThickness;

  return BoundingBox(origin.x - maxExtent, origin.y - maxExtent,
                     origin.x + maxExtent, origin.y + maxExtent);
}

void LineEntity::setPosition(const SDL_FPoint& position) {
  // Calculate the center of the line
  SDL_FPoint center = {(start.x + end.x) / 2.0f, (start.y + end.y) / 2.0f};

  // Calculate the offset to move the line
  float offsetX = position.x - center.x;
  float offsetY = position.y - center.y;

  // Move both start and end points by the offset
  start.x += offsetX;
  start.y += offsetY;
  end.x += offsetX;
  end.y += offsetY;

  // Update origin for rotation if it exists
  if (origin.x != 0.0f || origin.y != 0.0f) {
    origin.x += offsetX;
    origin.y += offsetY;
  }
}

SDL_FPoint LineEntity::getPosition() const {
  // Return the center of the line
  return SDL_FPoint{(start.x + end.x) / 2.0f, (start.y + end.y) / 2.0f};
}
