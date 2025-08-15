#include "ui.h"

#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_sdlrenderer3.h"
#include "core/app_state.h"
#include "entities/entity.h"
#include "entities/text.h"
#include "entities/triangle.h"

void UI::render() {
  // Clear the renderer at the start of each frame
  SDL_SetRenderScale(this->appState->context->renderer,
                     this->appState->io->DisplayFramebufferScale.x,
                     this->appState->io->DisplayFramebufferScale.y);
  SDL_SetRenderDrawColorFloat(this->appState->context->renderer, CLEAR_COLOR.x,
                              CLEAR_COLOR.y, CLEAR_COLOR.z, CLEAR_COLOR.w);
  SDL_RenderClear(this->appState->context->renderer);

  ImGui_ImplSDLRenderer3_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  ImGui::NewFrame();

  this->debug.render();

  // Render entities
  this->appState->entityManager.render(this->appState->context->renderer);

  // Render debug frames if enabled
  if (this->debug.isDebugFramesEnabled()) {
    this->renderDebugFrames();
  }

  // Render ImGui
  ImGui::Render();
  ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(),
                                        this->appState->context->renderer);

  SDL_RenderPresent(this->appState->context->renderer);
}

void UI::renderDebugFrames() {
  // Set green color for debug frames
  SDL_SetRenderDrawColor(this->appState->context->renderer, 0, 255, 0, 255);

  // Get all entities and draw debug frames around them
  auto entities = this->appState->entityManager.getAllEntities();

  for (auto* entity : entities) {
    if (entity->isVisible()) {
      BoundingBox bbox = entity->getBoundingBox();

      // Draw rectangle from minX,minY to maxX,maxY
      SDL_FRect debugRect = {bbox.minX, bbox.minY, bbox.maxX - bbox.minX,
                             bbox.maxY - bbox.minY};

      // Draw the debug frame rectangle
      SDL_RenderRect(this->appState->context->renderer, &debugRect);

      // Render debug information above the bounding box
      renderDebugInfo(entity);
    }
  }
}

std::vector<std::string> UI::getDebugText(Entity* entity) {
  BoundingBox bbox = entity->getBoundingBox();
  std::vector<std::string> debugText;

  debugText.push_back("UUID: " + entity->getUUID());
  debugText.push_back("Pos: (" + std::to_string(static_cast<int>(bbox.minX)) +
                      ", " + std::to_string(static_cast<int>(bbox.minY)) + ")");
  debugText.push_back("Type: " +
                      EntityTypeRegistry::getTypeName(entity->getEntityType()));
  debugText.push_back(
      "Size: " + std::to_string(static_cast<int>(bbox.maxX - bbox.minX)) + "x" +
      std::to_string(static_cast<int>(bbox.maxY - bbox.minY)));

  return debugText;
}

void UI::renderDebugInfo(Entity* entity) {
  BoundingBox bbox = entity->getBoundingBox();
  int textX = static_cast<int>(bbox.minX);
  int textY = static_cast<int>(bbox.minY) - 100;

  std::vector<std::string> debugText = getDebugText(entity);

  SDL_Color debugTextColor = {255, 255, 255, 255};  // White

  for (const auto& line : debugText) {
    this->appState->renderer.renderText(line, textX, textY, debugTextColor);
    textY += 20;
  }
}

void UI::createDemoEntities() {
  int windowWidth, windowHeight;
  SDL_GetWindowSize(this->appState->context->window, &windowWidth,
                    &windowHeight);

  // Create a demo triangle entity
  auto* triangle = this->appState->entityManager.createEntity<TriangleEntity>(
      SDL_FPoint{200, 200}, SDL_FPoint{250, 150}, SDL_FPoint{300, 200});
  triangle->setColor({255, 0, 255, 255});  // Magenta
  triangle->setZOrder(5.0f);

  // Create a demo text entity
  auto* text = this->appState->entityManager.createEntity<TextEntity>(
      "Triangle Demo", 150, 250, SDL_Color{255, 255, 255, 255});
  text->setZOrder(10.0f);
}
