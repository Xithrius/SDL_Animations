#include "settings.h"

#include "event_loop.h"

void SettingsUI::MainOptions() {
  ImGui::SeparatorText("Framerate Controls");

  float targetFPS = getUI()->getEventLoop()->getTargetFPS();

  if (ImGui::SliderFloat("Target FPS", &targetFPS, 0.0f, 240.0f, "%.0f")) {
    getUI()->getEventLoop()->setTargetFPS(targetFPS);
  }

  ImGui::Text("Current FPS: %.1f", this->getAppState()->io->Framerate);

  if (ImGui::Button("30 FPS")) {
    targetFPS = 30.0f;
    getUI()->getEventLoop()->setTargetFPS(targetFPS);
  }
  ImGui::SameLine();
  if (ImGui::Button("60 FPS")) {
    targetFPS = 60.0f;
    getUI()->getEventLoop()->setTargetFPS(targetFPS);
  }
  ImGui::SameLine();
  if (ImGui::Button("120 FPS")) {
    targetFPS = 120.0f;
    getUI()->getEventLoop()->setTargetFPS(targetFPS);
  }
  ImGui::SameLine();
  if (ImGui::Button("Unlimited")) {
    targetFPS = 0.0f;
    getUI()->getEventLoop()->setTargetFPS(targetFPS);
  }
}

void SettingsUI::render() {
  if (!this->visible) return;

  ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);
  ImGui::Begin("Settings", &this->visible);

  this->MainOptions();

  ImGui::End();
}
