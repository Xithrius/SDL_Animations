#include "audio_ui.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>

AudioUI::AudioUI()
    : audioSystem(nullptr),
      showFileDialog(false),
      showVisualization(true),
      showFrequencyBands(true),
      showBeatDetection(true),
      selectedFFTSize(2048),
      visualizationScale(1.0f),
      autoScale(true) {
  filePathBuffer[0] = '\0';
}

AudioUI::~AudioUI() {}

void AudioUI::render() {
  if (!audioSystem) return;

  ImGui::Begin("Audio System", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

  renderFileControls();
  ImGui::Separator();
  renderPlaybackControls();
  ImGui::Separator();
  renderAudioInfo();

  if (showVisualization) {
    ImGui::Separator();
    renderVisualizationSettings();

    if (showFrequencyBands) {
      ImGui::Separator();
      renderFrequencyBands();
    }

    if (showBeatDetection) {
      ImGui::Separator();
      renderBeatDetection();
    }
  }

  ImGui::End();

  if (showFileDialog) {
    renderFileDialog();
  }
}

void AudioUI::renderFileControls() {
  ImGui::Text("File Controls");

  if (ImGui::Button("Load Audio File")) {
    showFileDialog = true;
  }

  ImGui::SameLine();

  if (ImGui::Button("Clear Audio")) {
    audioSystem->stopPlayback();
    // Note: We'd need to add a clear method to AudioSystem
  }

  if (audioSystem->getAudioData().loaded) {
    ImGui::Text("Loaded: %s", filePathBuffer);
  } else {
    ImGui::Text("No audio file loaded");
  }
}

void AudioUI::renderPlaybackControls() {
  ImGui::Text("Playback Controls");

  const auto& audioData = audioSystem->getAudioData();
  if (!audioData.loaded) {
    ImGui::Text("No audio loaded");
    return;
  }

  // Play/Pause button
  if (audioSystem->isPlaying()) {
    if (ImGui::Button("Pause")) {
      audioSystem->pausePlayback();
    }
  } else {
    if (ImGui::Button("Play")) {
      audioSystem->startPlayback();
    }
  }

  ImGui::SameLine();

  if (ImGui::Button("Stop")) {
    audioSystem->stopPlayback();
  }

  // Progress bar
  double position = audioSystem->getPlaybackPosition();
  double duration = audioSystem->getDuration();

  ImGui::Text("Position: %s / %s", formatTime(position).c_str(),
              formatTime(duration).c_str());

  float progress =
      (duration > 0) ? static_cast<float>(position / duration) : 0.0f;
  ImGui::ProgressBar(progress, ImVec2(-1, 0));

  // Time slider
  static float sliderPos = 0.0f;
  if (ImGui::SliderFloat("Seek", &sliderPos, 0.0f, 1.0f, "%.3f")) {
    // Note: We'd need to add a seek method to AudioSystem
  }
}

void AudioUI::renderAudioInfo() {
  ImGui::Text("Audio Information");

  const auto& audioData = audioSystem->getAudioData();
  if (!audioData.loaded) {
    ImGui::Text("No audio loaded");
    return;
  }

  ImGui::Text("Sample Rate: %d Hz", audioData.sampleRate);
  ImGui::Text("Channels: %d", audioData.channels);
  ImGui::Text("Total Samples: %d", audioData.totalSamples);
  ImGui::Text("Duration: %s",
              formatTime(audioData.totalSamples /
                         static_cast<double>(audioData.sampleRate))
                  .c_str());
}

void AudioUI::renderVisualizationSettings() {
  ImGui::Text("Visualization Settings");

  ImGui::Checkbox("Show Frequency Bands", &showFrequencyBands);
  ImGui::Checkbox("Show Beat Detection", &showBeatDetection);

  // FFT size selection
  const char* fftSizes[] = {"512", "1024", "2048", "4096", "8192"};
  int currentFFTSizeIndex = 0;
  switch (selectedFFTSize) {
    case 512:
      currentFFTSizeIndex = 0;
      break;
    case 1024:
      currentFFTSizeIndex = 1;
      break;
    case 2048:
      currentFFTSizeIndex = 2;
      break;
    case 4096:
      currentFFTSizeIndex = 3;
      break;
    case 8192:
      currentFFTSizeIndex = 4;
      break;
  }

  if (ImGui::Combo("FFT Size", &currentFFTSizeIndex, fftSizes,
                   IM_ARRAYSIZE(fftSizes))) {
    switch (currentFFTSizeIndex) {
      case 0:
        selectedFFTSize = 512;
        break;
      case 1:
        selectedFFTSize = 1024;
        break;
      case 2:
        selectedFFTSize = 2048;
        break;
      case 3:
        selectedFFTSize = 4096;
        break;
      case 4:
        selectedFFTSize = 8192;
        break;
    }
    audioSystem->setFFTSize(selectedFFTSize);
  }

  ImGui::SliderFloat("Visualization Scale", &visualizationScale, 0.1f, 5.0f,
                     "%.2f");
  ImGui::Checkbox("Auto Scale", &autoScale);
}

void AudioUI::renderFrequencyBands() {
  ImGui::Text("Frequency Analysis");

  const auto& vizData = audioSystem->getVisualizationData();

  // Frequency band meters
  ImGui::Text("Bass (20-250 Hz):");
  ImGui::ProgressBar(static_cast<float>(vizData.bassLevel), ImVec2(-1, 0));

  ImGui::Text("Low Mid (250-500 Hz):");
  ImGui::ProgressBar(static_cast<float>(vizData.lowMidLevel), ImVec2(-1, 0));

  ImGui::Text("Mid (500-2000 Hz):");
  ImGui::ProgressBar(static_cast<float>(vizData.midLevel), ImVec2(-1, 0));

  ImGui::Text("High Mid (2000-4000 Hz):");
  ImGui::ProgressBar(static_cast<float>(vizData.highMidLevel), ImVec2(-1, 0));

  ImGui::Text("Treble (4000-20000 Hz):");
  ImGui::ProgressBar(static_cast<float>(vizData.trebleLevel), ImVec2(-1, 0));

  // Numerical values
  ImGui::Text("Bass: %.3f", vizData.bassLevel);
  ImGui::SameLine();
  ImGui::Text("Low Mid: %.3f", vizData.lowMidLevel);
  ImGui::SameLine();
  ImGui::Text("Mid: %.3f", vizData.midLevel);
  ImGui::SameLine();
  ImGui::Text("High Mid: %.3f", vizData.highMidLevel);
  ImGui::SameLine();
  ImGui::Text("Treble: %.3f", vizData.trebleLevel);
}

void AudioUI::renderBeatDetection() {
  ImGui::Text("Beat Detection");

  const auto& vizData = audioSystem->getVisualizationData();

  // Beat indicator
  ImGui::Text("Beat Detected: %s", vizData.isBeat ? "YES" : "NO");

  // Beat intensity meter
  ImGui::Text("Beat Intensity:");
  ImGui::ProgressBar(static_cast<float>(vizData.beatIntensity), ImVec2(-1, 0));

  // Energy levels
  ImGui::Text("RMS Energy: %.3f", vizData.rmsEnergy);
  ImGui::Text("Current Peak: %.3f", vizData.currentPeak);
  ImGui::Text("Average Peak: %.3f", vizData.averagePeak);

  // Spectral analysis
  ImGui::Text("Spectral Centroid: %.1f Hz", vizData.spectralCentroid);
  ImGui::Text("Spectral Rolloff: %.1f Hz", vizData.spectralRolloff);

  // Peak history visualization
  if (!vizData.peakHistory.empty()) {
    ImGui::Text("Peak History:");
    ImGui::PlotLines(
        "##PeakHistory",
        [](void* data, int idx) -> float {
          auto* history = static_cast<std::deque<double>*>(data);
          if (idx < static_cast<int>(history->size())) {
            return static_cast<float>((*history)[idx]);
          }
          return 0.0f;
        },
        const_cast<std::deque<double>*>(&vizData.peakHistory),
        static_cast<int>(vizData.peakHistory.size()), 0, nullptr, 0.0f, 1.0f,
        ImVec2(200, 50));
  }

  // Energy history visualization
  if (!vizData.energyHistory.empty()) {
    ImGui::Text("Energy History:");
    ImGui::PlotLines(
        "##EnergyHistory",
        [](void* data, int idx) -> float {
          auto* history = static_cast<std::deque<double>*>(data);
          if (idx < static_cast<int>(history->size())) {
            return static_cast<float>((*history)[idx]);
          }
          return 0.0f;
        },
        const_cast<std::deque<double>*>(&vizData.energyHistory),
        static_cast<int>(vizData.energyHistory.size()), 0, nullptr, 0.0f, 1.0f,
        ImVec2(200, 50));
  }
}

void AudioUI::renderFileDialog() {
  ImGui::Begin("Load Audio File", &showFileDialog,
               ImGuiWindowFlags_AlwaysAutoResize);

  ImGui::Text("Enter file path:");
  ImGui::InputText("##FilePath", filePathBuffer, sizeof(filePathBuffer));

  ImGui::Separator();

  if (ImGui::Button("Load")) {
    if (strlen(filePathBuffer) > 0) {
      if (audioSystem->loadAudioFile(filePathBuffer)) {
        showFileDialog = false;
      }
    }
  }

  ImGui::SameLine();

  if (ImGui::Button("Cancel")) {
    showFileDialog = false;
  }

  ImGui::Text("Note: Only WAV files are supported");

  ImGui::End();
}

std::string AudioUI::formatTime(double seconds) const {
  int minutes = static_cast<int>(seconds) / 60;
  int secs = static_cast<int>(seconds) % 60;
  int millisecs =
      static_cast<int>((seconds - static_cast<int>(seconds)) * 1000);

  std::ostringstream oss;
  oss << std::setfill('0') << std::setw(2) << minutes << ":"
      << std::setfill('0') << std::setw(2) << secs << "." << std::setfill('0')
      << std::setw(3) << millisecs;

  return oss.str();
}

void AudioUI::drawSpectrum(const std::vector<double>& magnitudes,
                           const std::vector<double>& frequencies) {
  if (magnitudes.empty()) return;

  ImGui::Text("Frequency Spectrum");

  // Find max magnitude for scaling
  double maxMagnitude = 0.0;
  for (const auto& mag : magnitudes) {
    maxMagnitude = std::max(maxMagnitude, mag);
  }

  if (maxMagnitude <= 0.0) return;

  // Draw spectrum as bars
  ImGui::BeginChild("##Spectrum", ImVec2(400, 100), true);

  ImDrawList* drawList = ImGui::GetWindowDrawList();
  ImVec2 canvasPos = ImGui::GetCursorScreenPos();
  ImVec2 canvasSize = ImGui::GetContentRegionAvail();

  float barWidth = canvasSize.x / magnitudes.size();
  float scale = autoScale ? 1.0f : visualizationScale;

  for (size_t i = 0; i < magnitudes.size(); ++i) {
    float height =
        static_cast<float>(magnitudes[i] / maxMagnitude) * canvasSize.y * scale;
    height = std::min(height, canvasSize.y);

    ImVec2 barMin(canvasPos.x + i * barWidth,
                  canvasPos.y + canvasSize.y - height);
    ImVec2 barMax(canvasPos.x + (i + 1) * barWidth, canvasPos.y + canvasSize.y);

    // Color based on frequency
    ImU32 color = IM_COL32(255, 255, 255, 255);
    if (frequencies[i] < 250) {
      color = IM_COL32(255, 0, 0, 255);  // Red for bass
    } else if (frequencies[i] < 2000) {
      color = IM_COL32(0, 255, 0, 255);  // Green for mid
    } else {
      color = IM_COL32(0, 0, 255, 255);  // Blue for treble
    }

    drawList->AddRectFilled(barMin, barMax, color);
  }

  ImGui::EndChild();
}

void AudioUI::drawWaveform(const std::vector<float>& samples,
                           size_t startSample, size_t numSamples) {
  if (samples.empty() || numSamples == 0) return;

  ImGui::Text("Waveform");

  ImGui::BeginChild("##Waveform", ImVec2(400, 100), true);

  ImDrawList* drawList = ImGui::GetWindowDrawList();
  ImVec2 canvasPos = ImGui::GetCursorScreenPos();
  ImVec2 canvasSize = ImGui::GetContentRegionAvail();

  // Find max amplitude for scaling
  float maxAmplitude = 0.0f;
  for (size_t i = startSample;
       i < std::min(startSample + numSamples, samples.size()); ++i) {
    maxAmplitude = std::max(maxAmplitude, std::abs(samples[i]));
  }

  if (maxAmplitude <= 0.0f) return;

  // Draw waveform
  float scale = autoScale ? 1.0f : visualizationScale;
  float centerY = canvasPos.y + canvasSize.y * 0.5f;

  for (size_t i = 0; i < numSamples - 1 && startSample + i + 1 < samples.size();
       ++i) {
    float x1 = canvasPos.x + (i * canvasSize.x) / numSamples;
    float x2 = canvasPos.x + ((i + 1) * canvasSize.x) / numSamples;

    float y1 = centerY + (samples[startSample + i] / maxAmplitude) *
                             canvasSize.y * 0.5f * scale;
    float y2 = centerY + (samples[startSample + i + 1] / maxAmplitude) *
                             canvasSize.y * 0.5f * scale;

    drawList->AddLine(ImVec2(x1, y1), ImVec2(x2, y2),
                      IM_COL32(255, 255, 255, 255));
  }

  ImGui::EndChild();
}
