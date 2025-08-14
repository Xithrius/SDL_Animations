#pragma once

#include <SDL3/SDL.h>

#include <memory>
#include <vector>

class AppContext;

class Entity {
 protected:
  bool visible = true;
  bool active = true;
  float z_order = 0.0f;

 public:
  Entity() = default;
  virtual ~Entity() = default;

  // Core entity methods
  virtual void update(float) {}
  virtual void render(SDL_Renderer* renderer) = 0;

  // Entity state
  virtual void setVisible(bool visible) { this->visible = visible; }

  virtual void setActive(bool active) { this->active = active; }

  virtual void setZOrder(float z) { z_order = z; }

  bool isVisible() const { return visible; }

  bool isActive() const { return active; }

  float getZOrder() const { return z_order; }
};

// Entity manager for handling multiple entities
class EntityManager {
 private:
  std::vector<std::unique_ptr<Entity>> entities;
  std::vector<Entity*> entitiesToRemove;
  AppContext* context;

 public:
  explicit EntityManager(AppContext* context);

  // Entity management
  template <typename T, typename... Args>
  T* createEntity(Args&&... args) {
    auto entity = std::make_unique<T>(std::forward<Args>(args)...);
    T* ptr = entity.get();
    entities.push_back(std::move(entity));
    return ptr;
  }

  void removeEntity(Entity* entity);
  void clear();

  // Update and render all entities
  void update(float deltaTime);
  void render(SDL_Renderer* renderer);

  // Entity queries
  std::vector<Entity*> getEntitiesByType(const std::type_info& type);

  template <typename T>
  std::vector<T*> getEntitiesOfType() {
    std::vector<T*> result;
    for (auto& entity : entities) {
      if (auto* typed = dynamic_cast<T*>(entity.get())) {
        result.push_back(typed);
      }
    }
    return result;
  }

  size_t getEntityCount() const { return entities.size(); }
};
