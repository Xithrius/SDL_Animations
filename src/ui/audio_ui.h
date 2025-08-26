#pragma once

#include <memory>
#include <string>

#include "../systems/audio_system.h"

class AudioUI {
 public:
  AudioUI();
  ~AudioUI();

  // Render the audio UI
  void render();

  // Set the audio system to control
  void setAudioSystem(AudioSystem* audioSystem) {
    this->audioSystem = audioSystem;
  }

  // Get the audio system
  AudioSystem* getAudioSystem() const { return audioSystem; }

 private:
  AudioSystem* audioSystem;

  // UI state
  char filePathBuffer[256];
  bool showFileDialog;
  bool showVisualization;
  bool showFrequencyBands;
  bool showBeatDetection;

  // Visualization settings
  int selectedFFTSize;
  float visualizationScale;
  bool autoScale;

  // Helper functions
  void renderFileControls();
  void renderPlaybackControls();
  void renderVisualizationSettings();
  void renderFrequencyBands();
  void renderBeatDetection();
  void renderAudioInfo();
  void renderFileDialog();

  // Format time for display
  std::string formatTime(double seconds) const;

  // Draw frequency spectrum
  void drawSpectrum(const std::vector<double>& magnitudes,
                    const std::vector<double>& frequencies);

  // Draw waveform
  void drawWaveform(const std::vector<float>& samples, size_t startSample,
                    size_t numSamples);
};
