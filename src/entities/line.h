#pragma once

#include <SDL3/SDL.h>

#include <vector>

#include "entity.h"

class LineEntity : public Entity,
                   public IPositionable,
                   public IUpdatable,
                   public IInteractive {
 public:
  // Gradient properties
  struct GradientProperties {
    bool enabled = false;
    SDL_Color startColor = {255, 255, 255, 255};
    SDL_Color endColor = {0, 0, 255, 255};
    std::vector<SDL_Color> stops;  // Custom gradient stops
  };

 private:
  SDL_FPoint start;
  SDL_FPoint end;
  float thickness = 1.0f;
  SDL_Color color = {255, 255, 255, 255};

  GradientProperties gradientProps;

  // Rotation properties
  SDL_FPoint origin;           // Center point to rotate around
  float angle = 0.0f;          // Current rotation angle in radians
  float rotationSpeed = 1.0f;  // Rotation speed in radians per second
  float lineLength = 0.0f;     // Length of the line for rotation calculations
  bool draggable = true;       // Can be set to false to disable dragging

 public:
  LineEntity(const SDL_FPoint& start, const SDL_FPoint& end);

  void update(float deltaTime) override;

  void render(SDL_Renderer* renderer) override;

  // Entity type identification
  EntityType getEntityType() const override {
    static EntityType typeId = EntityTypeRegistry::registerType("Line");
    return typeId;
  }

  // Debug methods
  BoundingBox getBoundingBox() const override;

  // Interface implementations
  IPositionable* asPositionable() override { return this; }

  IUpdatable* asUpdatable() override { return this; }

  IInteractive* asInteractive() override { return this; }

  // IPositionable implementation
  void setPosition(const SDL_FPoint& position) override;
  SDL_FPoint getPosition() const override;

  // IInteractive implementation
  bool canBeDragged() const override { return draggable; }

  void setDraggable(bool draggable) { this->draggable = draggable; }

  // Line-specific methods
  void setStart(const SDL_FPoint& start) { this->start = start; }

  void setEnd(const SDL_FPoint& end) { this->end = end; }

  void setThickness(float thickness) { this->thickness = thickness; }

  void setColor(const SDL_Color& color) { this->color = color; }

  void setGradientProperties(const GradientProperties& props) {
    gradientProps = props;
  }

  // Rotation methods
  void setOrigin(const SDL_FPoint& origin) { this->origin = origin; }

  void setRotationSpeed(float speed) { rotationSpeed = speed; }

  void setAngle(float angle) { this->angle = angle; }

  SDL_FPoint getStart() const { return start; }

  SDL_FPoint getEnd() const { return end; }

  SDL_FPoint getOrigin() const { return origin; }

  float getAngle() const { return angle; }

  float getRotationSpeed() const { return rotationSpeed; }
};
