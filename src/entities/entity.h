#pragma once

#include <SDL3/SDL.h>

#include <memory>
#include <string>
#include <vector>

#include "utils/uuid.h"

class AppState;

struct BoundingBox {
  float minX, minY, maxX, maxY;

  BoundingBox(float minX, float minY, float maxX, float maxY)
      : minX(minX), minY(minY), maxX(maxX), maxY(maxY) {}
};

class Entity {
 protected:
  bool visible = true;
  bool active = true;
  float z_order = 0.0f;
  std::string uuid;
  AppState* appState = nullptr;

  // Protected method to access AppState (set by EntityManager)
  void setAppState(AppState* appState) { this->appState = appState; }

 public:
  Entity() : uuid(generateUUID()) {}

  virtual ~Entity() = default;

  // Core entity methods
  virtual void update(float deltaTime) = 0;

  virtual void render(SDL_Renderer* renderer) = 0;

  // Entity state
  virtual void setVisible(bool visible) { this->visible = visible; }

  virtual void setActive(bool active) { this->active = active; }

  virtual void setZOrder(float z) { z_order = z; }

  bool isVisible() const { return visible; }

  bool isActive() const { return active; }

  float getZOrder() const { return z_order; }

  const std::string& getUUID() const { return uuid; }

  virtual BoundingBox getBoundingBox() const = 0;

  // Friend class to allow EntityManager to set AppState
  friend class EntityManager;
};

// Entity manager for handling multiple entities
class EntityManager {
 private:
  std::vector<std::unique_ptr<Entity>> entities;
  std::vector<Entity*> entitiesToRemove;
  AppState* appState;

 public:
  explicit EntityManager(AppState* appState);

  // Entity management
  template <typename T, typename... Args>
  T* createEntity(Args&&... args) {
    auto entity = std::make_unique<T>(std::forward<Args>(args)...);
    T* ptr = entity.get();
    // Set the AppState for the entity so it can access the renderer
    ptr->setAppState(appState);
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
  std::vector<Entity*> getAllEntities();

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
