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

class EntityTypeRegistry {
 private:
  std::unordered_map<std::string, uint32_t> typeMap;
  std::vector<std::string> typeNames;
  uint32_t nextTypeId = 0;

  EntityTypeRegistry() = default;

 public:
  static EntityTypeRegistry& getInstance() {
    static EntityTypeRegistry instance;
    return instance;
  }

  EntityTypeRegistry(const EntityTypeRegistry&) = delete;
  EntityTypeRegistry& operator=(const EntityTypeRegistry&) = delete;

  uint32_t registerType(const std::string& typeName);
  std::string getTypeName(uint32_t typeId) const;
  uint32_t getTypeId(const std::string& typeName) const;
};

using EntityType = uint32_t;

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

class IAnimatable {
 public:
  virtual ~IAnimatable() = default;
  virtual void addAnimation(std::shared_ptr<class Animation> animation) = 0;
  virtual void removeAnimation(std::shared_ptr<class Animation> animation) = 0;
  virtual std::vector<std::shared_ptr<class Animation>> getAnimations()
      const = 0;
  virtual void clearAnimations() = 0;
};

class Entity {
 protected:
  bool visible = true;
  bool active = true;
  float z_order = 0.0f;
  std::string uuid;
  AppState* appState = nullptr;

  void setAppState(AppState* appState) { this->appState = appState; }

 public:
  Entity() : uuid(generateUUID()) {}

  virtual ~Entity() = default;

  virtual void render(SDL_Renderer* renderer) = 0;

  virtual EntityType getEntityType() const = 0;

  virtual void setVisible(bool visible) { this->visible = visible; }

  virtual void setActive(bool active) { this->active = active; }

  virtual void setZOrder(float z) { z_order = z; }

  bool isVisible() const { return visible; }

  bool isActive() const { return active; }

  float getZOrder() const { return z_order; }

  const std::string& getUUID() const { return uuid; }

  virtual void update(float) {}

  virtual BoundingBox getBoundingBox() const { return BoundingBox(0, 0, 0, 0); }

  virtual IPositionable* asPositionable() { return nullptr; }

  virtual IUpdatable* asUpdatable() { return nullptr; }

  virtual IInteractive* asInteractive() { return nullptr; }

  virtual IAnimatable* asAnimatable() { return nullptr; }

  friend class EntityManager;
};

class EntityManager {
 private:
  std::vector<std::unique_ptr<Entity>> entities;
  std::vector<Entity*> entitiesToRemove;
  AppState* appState;

 public:
  explicit EntityManager(AppState* appState);

  template <typename T, typename... Args>
  T* createEntity(Args&&... args) {
    auto entity = std::make_unique<T>(std::forward<Args>(args)...);
    T* ptr = entity.get();
    ptr->setAppState(appState);
    entities.push_back(std::move(entity));
    return ptr;
  }

  void removeEntity(Entity* entity);
  void clear();

  void update(float deltaTime);
  void render(SDL_Renderer* renderer);

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
