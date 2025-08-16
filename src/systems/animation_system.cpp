#include "animation_system.h"

#include <spdlog/spdlog.h>

#include <algorithm>

void AnimationSystem::update(float deltaTime) {
  // Update all active animations
  // This is a placeholder for future animation logic
  // For now, the entity manager handles entity updates directly
}

void AnimationSystem::addAnimation(std::shared_ptr<Animation> animation) {
  // Add animation to the system
  // This is a placeholder for future animation management
}

void AnimationSystem::removeAnimation(std::shared_ptr<Animation> animation) {
  // Remove animation from the system
  // This is a placeholder for future animation management
}

void AnimationSystem::clear() {
  // Clear all animations
  // This is a placeholder for future animation management
}
