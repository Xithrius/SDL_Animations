#pragma once

#include <SDL3/SDL.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "utils/uuid.h"

class AppState;

struct BoundingBox {
  float minX, minY, maxX, maxY;

  BoundingBox(float minX, float minY, float maxX, float maxY)
      : minX(minX), minY(minY), maxX(maxX), maxY(maxY) {}
};

// Entity type registration system - no need to modify this file for new
// entities
class EntityTypeRegistry {
 private:
  static std::unordered_map<std::string, uint32_t> typeMap;
  static std::vector<std::string> typeNames;
  static uint32_t nextTypeId;

 public:
  static uint32_t registerType(const std::string& typeName);
  static std::string getTypeName(uint32_t typeId);
  static uint32_t getTypeId(const std::string& typeName);
};

// EntityType is now just a uint32_t for efficiency
using EntityType = uint32_t;

// Optional interfaces that entities can implement
class IPositionable {
 public:
  virtual ~IPositionable() = default;
  virtual void setPosition(const SDL_FPoint& position) = 0;
  virtual SDL_FPoint getPosition() const = 0;
};

class IUpdatable {
 public:
  virtual ~IUpdatable() = default;
  virtual void update(float deltaTime) = 0;
};

class IInteractive {
 public:
  virtual ~IInteractive() = default;
  virtual bool canBeDragged() const = 0;

  virtual void onDragStart() {}

  virtual void onDragEnd() {}
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

  // Core entity methods - only render is required
  virtual void render(SDL_Renderer* renderer) = 0;

  // Entity type identification - replaces typeid
  virtual EntityType getEntityType() const = 0;

  // Entity state
  virtual void setVisible(bool visible) { this->visible = visible; }

  virtual void setActive(bool active) { this->active = active; }

  virtual void setZOrder(float z) { z_order = z; }

  bool isVisible() const { return visible; }

  bool isActive() const { return active; }

  float getZOrder() const { return z_order; }

  const std::string& getUUID() const { return uuid; }

  virtual void update(float) {}

  virtual BoundingBox getBoundingBox() const {
    // Default implementation returns empty bounding box
    // Entities should override if they need collision detection
    return BoundingBox(0, 0, 0, 0);
  }

  // Interface query methods
  virtual IPositionable* asPositionable() { return nullptr; }

  virtual IUpdatable* asUpdatable() { return nullptr; }

  virtual IInteractive* asInteractive() { return nullptr; }

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
  std::vector<Entity*> getEntitiesByType(EntityType type);
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
