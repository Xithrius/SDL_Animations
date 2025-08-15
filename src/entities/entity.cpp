#include "entity.h"

#include <spdlog/spdlog.h>

#include <algorithm>

#include "core/app_state.h"

// Initialize static members
std::unordered_map<std::string, uint32_t> EntityTypeRegistry::typeMap;
std::vector<std::string> EntityTypeRegistry::typeNames;
uint32_t EntityTypeRegistry::nextTypeId = 0;

uint32_t EntityTypeRegistry::registerType(const std::string& typeName) {
  auto it = typeMap.find(typeName);
  if (it != typeMap.end()) {
    return it->second;  // Type already registered
  }

  uint32_t typeId = nextTypeId++;
  typeMap[typeName] = typeId;
  typeNames.push_back(typeName);
  return typeId;
}

std::string EntityTypeRegistry::getTypeName(uint32_t typeId) {
  if (typeId < typeNames.size()) {
    return typeNames[typeId];
  }
  return "Unknown";
}

uint32_t EntityTypeRegistry::getTypeId(const std::string& typeName) {
  auto it = typeMap.find(typeName);
  if (it != typeMap.end()) {
    return it->second;
  }
  return UINT32_MAX;  // Invalid type ID
}

EntityManager::EntityManager(AppState* appState) : appState(appState) {}

void EntityManager::removeEntity(Entity* entity) {
  entitiesToRemove.push_back(entity);
}

void EntityManager::clear() {
  entities.clear();
  entitiesToRemove.clear();
}

void EntityManager::update(float deltaTime) {
  // Remove marked entities
  if (!entitiesToRemove.empty()) {
    entities.erase(
        std::remove_if(
            entities.begin(), entities.end(),
            [this](const std::unique_ptr<Entity>& entity) {
              return std::find(entitiesToRemove.begin(), entitiesToRemove.end(),
                               entity.get()) != entitiesToRemove.end();
            }),
        entities.end());
    entitiesToRemove.clear();
  }

  // Update all active entities
  for (auto& entity : entities) {
    if (entity->isActive()) {
      entity->update(deltaTime);
    }
  }
}

void EntityManager::render(SDL_Renderer* renderer) {
  // Sort entities by z-order for proper layering
  std::vector<Entity*> sortedEntities;
  sortedEntities.reserve(entities.size());

  for (auto& entity : entities) {
    if (entity->isVisible()) {
      sortedEntities.push_back(entity.get());
    }
  }

  std::sort(sortedEntities.begin(), sortedEntities.end(),
            [](const Entity* a, const Entity* b) {
              return a->getZOrder() < b->getZOrder();
            });

  // Render entities in z-order
  for (Entity* entity : sortedEntities) {
    entity->render(renderer);
  }
}

std::vector<Entity*> EntityManager::getEntitiesByType(EntityType type) {
  std::vector<Entity*> result;
  for (auto& entity : entities) {
    if (entity && entity->getEntityType() == type) {
      result.push_back(entity.get());
    }
  }
  return result;
}

std::vector<Entity*> EntityManager::getAllEntities() {
  std::vector<Entity*> result;
  result.reserve(entities.size());

  for (auto& entity : entities) {
    result.push_back(entity.get());
  }

  return result;
}
