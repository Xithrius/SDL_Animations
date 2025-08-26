#pragma once

#include <SDL3/SDL.h>
#include <fftw3.h>

#include <complex>
#include <deque>
#include <memory>
#include <string>
#include <vector>

class AudioSystem {
 public:
  struct AudioData {
    std::vector<Sint16>
        samples;  // Store original Sint16 samples to avoid conversion artifacts
    int sampleRate;
    int channels;
    size_t totalSamples;
    bool loaded;
  };

  struct FFTResult {
    std::vector<std::complex<double>> spectrum;
    std::vector<double> magnitudes;
    std::vector<double> frequencies;
    int fftSize;
  };

  struct VisualizationData {
    // Peak detection
    double currentPeak;
    double averagePeak;
    double peakThreshold;

    // Frequency analysis
    double bassLevel;     // 20-250 Hz
    double lowMidLevel;   // 250-500 Hz
    double midLevel;      // 500-2000 Hz
    double highMidLevel;  // 2000-4000 Hz
    double trebleLevel;   // 4000-20000 Hz

    // Beat detection
    double beatIntensity;
    bool isBeat;
    double tempoEstimate;

    // Energy levels
    double rmsEnergy;
    double spectralCentroid;
    double spectralRolloff;

    // History for smoothing
    std::deque<double> peakHistory;
    std::deque<double> energyHistory;

    // Spectral flux history for beat detection
    std::vector<double> lastMagnitudes;  // previous frame magnitudes
    std::deque<double> spectralFluxHistory;
    double spectralFlux = 0.0;
    double spectralFluxEMA = 0.0;  // smoothed flux for robust detection
    Uint64 lastBeatMs = 0;         // refractory timer
  };

  AudioSystem();
  ~AudioSystem();

  // Load audio file (WAV format supported by SDL)
  bool loadAudioFile(const std::string& filename);

  // Start/stop audio playback
  void startPlayback();
  void stopPlayback();
  void pausePlayback();

  // Get current playback position
  double getPlaybackPosition() const;
  double getDuration() const;

  // Perform FFT analysis on current audio buffer
  FFTResult performFFT(const std::vector<float>& audioBuffer);

  // Update visualization data
  void updateVisualizationData();

  // Get current visualization data
  const VisualizationData& getVisualizationData() const { return vizData; }

  // Get raw audio data
  const AudioData& getAudioData() const { return audioData; }

  // Check if audio is playing
  bool isPlaying() const { return playing; }

  // Set FFT size (must be power of 2)
  void setFFTSize(int size);

  // Update playback (call this in your main loop)
  void updatePlayback();

 private:
  AudioData audioData;
  VisualizationData vizData;

  // FFTW3 plans and buffers
  fftw_plan fftPlan;
  fftw_complex* fftIn;
  fftw_complex* fftOut;
  int fftSize;

  // Audio playback
  SDL_AudioStream* audioStream;
  Uint8* wavData;
  Uint32 wavDataLen;
  SDL_AudioSpec originalSpec;
  bool playing;
  bool paused;
  size_t currentSample;
  Uint64 playbackStartTime;

  // FFT buffer for visualization
  std::vector<float> fftBuffer;

  // Helper functions
  void initializeFFT();
  void cleanupFFT();
  void calculateFrequencyBands(const FFTResult& fft, VisualizationData& data);
  void detectBeat(VisualizationData& data);
  void updatePeakHistory(VisualizationData& data);

  // Static audio callback for SDL3
  static void audioCallback(void* userdata, SDL_AudioStream* stream,
                            int additional_amount, int total_amount);
};
