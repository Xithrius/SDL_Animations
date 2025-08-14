#include "graphics.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <spdlog/spdlog.h>

#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_sdlrenderer3.h"
#include "constants.h"
#include "context.h"
#include "entities/circle.h"
#include "entities/entity.h"
#include "entities/line.h"
#include "entities/point.h"

Graphics::Graphics(AppContext* context) : context(context) {
  this->context->points.resize(POINT_COUNT);
  for (int i = 0; i < POINT_COUNT; i++) {
    this->context->point_speeds.push_back(SDL_randf() * MAX_POINT_SPEED +
                                          MIN_POINT_SPEED);

    this->context->points[i].resize(TRAIL_LENGTH);

    float initial_x = 0;
    float initial_y = 0;

    initial_x = SDL_randf() * WINDOW_WIDTH;
    initial_y = SDL_randf() * WINDOW_HEIGHT;

    for (int j = 0; j < TRAIL_LENGTH; j++) {
      this->context->points[i][j].x = initial_x - j;
      this->context->points[i][j].y = initial_y - j;
    }
  }
}

void Graphics::renderText(const std::string& text, int x, int y,
                          SDL_Color color) {
  // Check if font is loaded
  if (!this->context->font.get()) {
    SPDLOG_WARN("Font not loaded, skipping text rendering");
    return;
  }

  SDL_Surface* text_surface = TTF_RenderText_Blended(
      this->context->font.get(), text.c_str(), text.length(), color);
  if (!text_surface) {
    SPDLOG_ERROR("Couldn't render text: %s", SDL_GetError());
    return;
  }

  SDL_Texture* text_texture =
      SDL_CreateTextureFromSurface(this->context->renderer, text_surface);
  if (!text_texture) {
    SPDLOG_ERROR("Couldn't create texture from surface: %s", SDL_GetError());
    SDL_DestroySurface(text_surface);
    return;
  }

  SDL_FRect dest_rect = {(float)x, (float)y, (float)text_surface->w,
                         (float)text_surface->h};
  SDL_RenderTexture(this->context->renderer, text_texture, nullptr, &dest_rect);

  SDL_DestroySurface(text_surface);
  SDL_DestroyTexture(text_texture);
}

void Graphics::render(float tickRate) {
  SDL_RenderClear(this->context->renderer);

  this->renderEntities();
  this->renderUI(tickRate);
}

void Graphics::renderEntities() {
  // Use the new entity manager to render all entities
  this->context->entityManager.render(this->context->renderer);
}

void Graphics::renderUI(float tickRate) {
  ImGui_ImplSDLRenderer3_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  ImGui::NewFrame();

  {
    if (this->show_debug) {
      ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);
      ImGui::Begin("Settings", &this->show_debug);

      ImGui::SliderFloat("Point Speed Multiplier",
                         &this->context->point_speed_multiplier, 0.0f, 10.0f);
      ImGui::ColorEdit3("clear color", (float*)&clear_color);

      ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                  1000.0f / this->context->io->Framerate,
                  this->context->io->Framerate);

      ImGui::Text("Event Logic: %d Hz", static_cast<int>(tickRate));

      // Entity system controls
      ImGui::Separator();
      ImGui::Text("Entity System:");

      static bool newEntitySystemDemo = false;
      if (ImGui::Checkbox("New entity system demo", &newEntitySystemDemo)) {
        if (newEntitySystemDemo) {
          this->createDemoEntities();
        } else {
          this->clearEntities();
        }
      }

      ImGui::Text("Entity Count: %zu",
                  this->context->entityManager.getEntityCount());

      if (ImGui::Button("Add Random Line")) {
        int windowWidth, windowHeight;
        SDL_GetWindowSize(this->context->window, &windowWidth, &windowHeight);

        auto* line = this->context->entityManager.createEntity<LineEntity>(
            SDL_FPoint{SDL_randf() * windowWidth, SDL_randf() * windowHeight},
            SDL_FPoint{SDL_randf() * windowWidth, SDL_randf() * windowHeight});

        LineEntity::GradientProperties gradientProps;
        gradientProps.enabled = true;
        gradientProps.startColor = {static_cast<Uint8>(SDL_randf() * 255),
                                    static_cast<Uint8>(SDL_randf() * 255),
                                    static_cast<Uint8>(SDL_randf() * 255), 255};
        gradientProps.endColor = {static_cast<Uint8>(SDL_randf() * 255),
                                  static_cast<Uint8>(SDL_randf() * 255),
                                  static_cast<Uint8>(SDL_randf() * 255), 255};
        line->setGradientProperties(gradientProps);
      }

      if (ImGui::Button("Add Random Circle")) {
        int windowWidth, windowHeight;
        SDL_GetWindowSize(this->context->window, &windowWidth, &windowHeight);

        auto* circle = this->context->entityManager.createEntity<CircleEntity>(
            SDL_FPoint{SDL_randf() * windowWidth, SDL_randf() * windowHeight},
            SDL_randf() * 50.0f + 10.0f);
        circle->setColor({static_cast<Uint8>(SDL_randf() * 255),
                          static_cast<Uint8>(SDL_randf() * 255),
                          static_cast<Uint8>(SDL_randf() * 255), 128});
        circle->setFilled(SDL_randf() > 0.5f);
      }

      if (ImGui::Button("Clear All Entities")) {
        this->clearEntities();
      }

      ImGui::End();
    }
  }

  ImGui::Render();
  SDL_SetRenderScale(this->context->renderer,
                     this->context->io->DisplayFramebufferScale.x,
                     this->context->io->DisplayFramebufferScale.y);
  SDL_SetRenderDrawColorFloat(this->context->renderer, clear_color.x,
                              clear_color.y, clear_color.z, clear_color.w);
  ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(),
                                        this->context->renderer);

  SDL_RenderPresent(this->context->renderer);
}

void Graphics::createDemoEntities() {
  // Create some demo entities to show the new system
  int windowWidth, windowHeight;
  SDL_GetWindowSize(this->context->window, &windowWidth, &windowHeight);

  // Create animated points with trails
  for (int i = 0; i < POINT_COUNT; ++i) {
    auto* point = this->context->entityManager.createEntity<PointEntity>(
        this->context, TRAIL_LENGTH,
        SDL_randf() * MAX_POINT_SPEED + MIN_POINT_SPEED);

    // Set initial position
    float x = SDL_randf() * windowWidth;
    float y = SDL_randf() * windowHeight;
    point->setInitialPosition(x, y);

    // Configure trail properties
    PointEntity::TrailProperties trailProps;
    trailProps.enabled = true;
    trailProps.startColor = {255, 255, 255, 255};
    trailProps.endColor = {0, 0, 0, 0};
    point->setTrailProperties(trailProps);
  }

  // Create some gradient lines
  auto* line1 = this->context->entityManager.createEntity<LineEntity>(
      SDL_FPoint{100, 100}, SDL_FPoint{300, 200});
  LineEntity::GradientProperties gradientProps;
  gradientProps.enabled = true;
  gradientProps.startColor = {255, 0, 0, 255};  // Red
  gradientProps.endColor = {0, 0, 255, 255};    // Blue
  line1->setGradientProperties(gradientProps);

  // Create a circle
  auto* circle = this->context->entityManager.createEntity<CircleEntity>(
      SDL_FPoint{windowWidth / 2.0f, windowHeight / 2.0f}, 50.0f);
  circle->setColor({0, 255, 0, 128});  // Semi-transparent green
  circle->setFilled(false);
  circle->setZOrder(1.0f);  // Render on top
}

void Graphics::clearEntities() { this->context->entityManager.clear(); }
