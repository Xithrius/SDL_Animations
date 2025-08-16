#include "animation_system.h"

#include <spdlog/spdlog.h>

#include <algorithm>

void AnimationSystem::update(float deltaTime) {
  // Remove finished animations
  animations.erase(std::remove_if(animations.begin(), animations.end(),
                                  [](const std::shared_ptr<Animation>& anim) {
                                    return anim->isFinished() &&
                                           !anim->isLooping();
                                  }),
                   animations.end());

  // Update all active animations
  for (auto& animation : animations) {
    animation->update(deltaTime);

    // Handle looping animations
    if (animation->isFinished() && animation->isLooping()) {
      animation->reset();
    }
  }
}

void AnimationSystem::addAnimation(std::shared_ptr<Animation> animation) {
  if (animation) {
    animations.push_back(animation);
  }
}

void AnimationSystem::removeAnimation(std::shared_ptr<Animation> animation) {
  auto it = std::find(animations.begin(), animations.end(), animation);

  if (it != animations.end()) {
    animations.erase(it);
  }
}

void AnimationSystem::clear() { animations.clear(); }

std::vector<std::shared_ptr<Animation>> AnimationSystem::getAnimationsForEntity(
    Entity* entity) {
  std::vector<std::shared_ptr<Animation>> result;

  for (auto& animation : animations) {
    if (animation->getTargetEntity() == entity) {
      result.push_back(animation);
    }
  }

  return result;
}
