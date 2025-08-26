#include "audio_system.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <numeric>

AudioSystem::AudioSystem()
    : fftPlan(nullptr),
      fftIn(nullptr),
      fftOut(nullptr),
      fftSize(2048),
      audioStream(nullptr),
      playing(false),
      paused(false),
      currentSample(0) {
  std::cout << "AudioSystem: Initializing..." << std::endl;

  // Initialize audio data
  audioData.loaded = false;

  // Initialize visualization data
  vizData.currentPeak = 0.0;
  vizData.averagePeak = 0.0;
  vizData.peakThreshold = 0.1;
  vizData.bassLevel = 0.0;
  vizData.lowMidLevel = 0.0;
  vizData.midLevel = 0.0;
  vizData.highMidLevel = 0.0;
  vizData.trebleLevel = 0.0;
  vizData.beatIntensity = 0.0;
  vizData.isBeat = false;
  vizData.tempoEstimate = 120.0;
  vizData.rmsEnergy = 0.0;
  vizData.spectralCentroid = 0.0;
  vizData.spectralRolloff = 0.0;

  // Initialize FFT
  std::cout << "AudioSystem: Initializing FFT..." << std::endl;
  initializeFFT();

  // Initialize callback data
  callbackData.system = this;
  callbackData.bufferSize = fftSize;
  callbackData.buffer.resize(fftSize, 0.0f);

  std::cout << "AudioSystem: Initialization complete" << std::endl;
}

AudioSystem::~AudioSystem() {
  std::cout << "AudioSystem: Destructing..." << std::endl;
  stopPlayback();
  cleanupFFT();
}

bool AudioSystem::loadAudioFile(const std::string& filename) {
  std::cout << "AudioSystem: Loading audio file: " << filename << std::endl;

  // Check file extension
  std::string extension = filename.substr(filename.find_last_of(".") + 1);
  std::transform(extension.begin(), extension.end(), extension.begin(),
                 ::tolower);

  if (extension != "wav") {
    std::cerr << "Unsupported file format: " << extension
              << ". Only WAV files are supported." << std::endl;
    return false;
  }

  SDL_IOStream* io = SDL_IOFromFile(filename.c_str(), "rb");
  if (!io) {
    std::cerr << "Failed to open audio file: " << filename << std::endl;
    return false;
  }

  SDL_AudioSpec spec;
  Uint8* audioBuffer = nullptr;
  Uint32 audioLength = 0;

  std::cout << "AudioSystem: About to call SDL_LoadWAV_IO..." << std::endl;
  if (!SDL_LoadWAV_IO(io, 1, &spec, &audioBuffer, &audioLength)) {
    std::cerr << "Failed to load WAV file: " << SDL_GetError() << std::endl;
    // Note: SDL_LoadWAV_IO automatically closes the IO stream in SDL3
    return false;
  }

  std::cout << "AudioSystem: WAV loaded successfully" << std::endl;
  std::cout << "  Sample rate: " << spec.freq << std::endl;
  std::cout << "  Channels: " << spec.channels << std::endl;
  std::cout << "  Format: " << spec.format << std::endl;
  std::cout << "  Audio length: " << audioLength << " bytes" << std::endl;
  std::cout << "  Audio buffer pointer: " << (void*)audioBuffer << std::endl;

  // Convert audio to float samples
  audioData.sampleRate = spec.freq;
  audioData.channels = spec.channels;
  audioData.totalSamples =
      audioLength / (spec.channels * (spec.format == SDL_AUDIO_S16 ? 2 : 1));

  std::cout << "AudioSystem: Total samples: " << audioData.totalSamples
            << std::endl;

  audioData.samples.resize(audioData.totalSamples);

  // Convert to mono if stereo
  if (spec.channels == 2) {
    Sint16* samples = reinterpret_cast<Sint16*>(audioBuffer);
    int totalSamples16 = audioLength / sizeof(Sint16);

    std::cout << "AudioSystem: Converting stereo to mono..." << std::endl;
    std::cout << "  Total 16-bit samples: " << totalSamples16 << std::endl;
    std::cout << "  Expected mono samples: " << audioData.totalSamples
              << std::endl;

    // Ensure we don't go out of bounds
    int maxSamples = std::min(audioData.totalSamples, totalSamples16 / 2);

    for (int i = 0; i < maxSamples; ++i) {
      if (i * 2 + 1 < totalSamples16) {
        float left = samples[i * 2] / 32768.0f;
        float right = samples[i * 2 + 1] / 32768.0f;
        audioData.samples[i] = (left + right) * 0.5f;
      } else {
        // Pad with zeros if we run out of data
        audioData.samples[i] = 0.0f;
      }
    }

    // Fill remaining samples with zeros if needed
    for (int i = maxSamples; i < audioData.totalSamples; ++i) {
      audioData.samples[i] = 0.0f;
    }

    std::cout << "AudioSystem: Converted " << maxSamples
              << " stereo samples to mono" << std::endl;
  } else {
    Sint16* samples = reinterpret_cast<Sint16*>(audioBuffer);
    int totalSamples16 = audioLength / sizeof(Sint16);

    std::cout << "AudioSystem: Converting mono samples..." << std::endl;
    std::cout << "  Total 16-bit samples: " << totalSamples16 << std::endl;

    // Ensure we don't go out of bounds
    int maxSamples = std::min(audioData.totalSamples, totalSamples16);

    for (int i = 0; i < maxSamples; ++i) {
      audioData.samples[i] = samples[i] / 32768.0f;
    }

    // Fill remaining samples with zeros if needed
    for (int i = maxSamples; i < audioData.totalSamples; ++i) {
      audioData.samples[i] = 0.0f;
    }

    std::cout << "AudioSystem: Converted " << maxSamples << " mono samples"
              << std::endl;
  }

  std::cout << "AudioSystem: About to free audio buffer..." << std::endl;
  std::cout << "  Audio buffer pointer: " << (void*)audioBuffer << std::endl;

  if (audioBuffer) {
    SDL_free(audioBuffer);
    std::cout << "AudioSystem: Audio buffer freed successfully" << std::endl;
  } else {
    std::cout << "AudioSystem: Warning: audioBuffer was null" << std::endl;
  }

  // Note: SDL_LoadWAV_IO automatically closes the IO stream in SDL3
  // so we don't need to call SDL_CloseIO(io) here
  std::cout
      << "AudioSystem: IO stream was automatically closed by SDL_LoadWAV_IO"
      << std::endl;

  audioData.loaded = true;

  std::cout << "AudioSystem: Audio file loaded successfully" << std::endl;
  std::cout << "  Samples: " << audioData.totalSamples
            << ", Rate: " << audioData.sampleRate
            << ", Channels: " << audioData.channels << std::endl;

  return true;
}

void AudioSystem::startPlayback() {
  if (!audioData.loaded || playing) return;

  // For now, we'll use a simpler approach without audio streaming
  // since the SDL3 audio callback API is different
  currentSample = 0;
  playing = true;
  paused = false;

  std::cout << "Audio playback started (simulated)" << std::endl;
}

void AudioSystem::stopPlayback() {
  if (playing) {
    playing = false;
    paused = false;
    currentSample = 0;
    std::cout << "Audio playback stopped" << std::endl;
  }
}

void AudioSystem::pausePlayback() {
  if (playing && !paused) {
    paused = true;
    std::cout << "Audio playback paused" << std::endl;
  } else if (playing && paused) {
    paused = false;
    std::cout << "Audio playback resumed" << std::endl;
  }
}

double AudioSystem::getPlaybackPosition() const {
  if (!audioData.loaded) return 0.0;
  return static_cast<double>(currentSample) / audioData.sampleRate;
}

double AudioSystem::getDuration() const {
  if (!audioData.loaded) return 0.0;
  return static_cast<double>(audioData.totalSamples) / audioData.sampleRate;
}

void AudioSystem::initializeFFT() {
  std::cout << "AudioSystem: Allocating FFT buffers..." << std::endl;

  fftIn = fftw_alloc_complex(fftSize);
  fftOut = fftw_alloc_complex(fftSize);

  if (!fftIn || !fftOut) {
    std::cerr << "AudioSystem: Failed to allocate FFT buffers!" << std::endl;
    return;
  }

  std::cout << "AudioSystem: Creating FFT plan..." << std::endl;
  fftPlan =
      fftw_plan_dft_1d(fftSize, fftIn, fftOut, FFTW_FORWARD, FFTW_ESTIMATE);

  if (!fftPlan) {
    std::cerr << "AudioSystem: Failed to create FFT plan!" << std::endl;
    return;
  }

  std::cout << "AudioSystem: FFT initialization complete" << std::endl;
}

void AudioSystem::cleanupFFT() {
  if (fftPlan) {
    fftw_destroy_plan(fftPlan);
    fftPlan = nullptr;
  }
  if (fftIn) {
    fftw_free(fftIn);
    fftIn = nullptr;
  }
  if (fftOut) {
    fftw_free(fftOut);
    fftOut = nullptr;
  }
}

AudioSystem::FFTResult AudioSystem::performFFT(
    const std::vector<float>& audioBuffer) {
  FFTResult result;
  result.fftSize = fftSize;

  // Check if FFT is properly initialized
  if (!fftPlan || !fftIn || !fftOut) {
    std::cerr << "AudioSystem: FFT not properly initialized!" << std::endl;
    return result;
  }

  // Apply window function (Hann window) and copy to FFT input
  for (int i = 0; i < fftSize; ++i) {
    float window = 0.5f * (1.0f - cos(2.0f * M_PI * i / (fftSize - 1)));
    fftIn[i][0] = (static_cast<size_t>(i) < audioBuffer.size())
                      ? audioBuffer[i] * window
                      : 0.0;
    fftIn[i][1] = 0.0;
  }

  // Perform FFT
  fftw_execute(fftPlan);

  // Process results
  result.spectrum.resize(fftSize / 2);
  result.magnitudes.resize(fftSize / 2);
  result.frequencies.resize(fftSize / 2);

  for (int i = 0; i < fftSize / 2; ++i) {
    double real = fftOut[i][0];
    double imag = fftOut[i][1];
    result.spectrum[i] = std::complex<double>(real, imag);
    result.magnitudes[i] = std::sqrt(real * real + imag * imag);
    result.frequencies[i] =
        static_cast<double>(i) * audioData.sampleRate / fftSize;
  }

  return result;
}

void AudioSystem::updateVisualizationData() {
  // Initialize buffer if empty
  if (callbackData.buffer.empty()) {
    callbackData.buffer.resize(fftSize, 0.0f);
  }

  if (!playing) {
    // If not playing, just return with zero data
    return;
  }

  // If audio is loaded and playing, simulate audio data by reading from the
  // loaded samples
  if (audioData.loaded && playing) {
    // Simulate audio data by reading from the loaded samples
    size_t startSample = currentSample;
    size_t numSamples = std::min(static_cast<size_t>(fftSize),
                                 audioData.samples.size() - startSample);

    if (numSamples > 0) {
      callbackData.buffer.assign(
          audioData.samples.begin() + startSample,
          audioData.samples.begin() + startSample + numSamples);

      // Pad with zeros if needed
      while (callbackData.buffer.size() < static_cast<size_t>(fftSize)) {
        callbackData.buffer.push_back(0.0f);
      }

      // Advance sample position
      currentSample += numSamples;
      if (currentSample >= audioData.samples.size()) {
        currentSample = 0;  // Loop
      }
    } else {
      // No samples available, fill with zeros
      std::fill(callbackData.buffer.begin(), callbackData.buffer.end(), 0.0f);
    }
  } else {
    // No audio loaded, fill with zeros
    std::fill(callbackData.buffer.begin(), callbackData.buffer.end(), 0.0f);
  }

  // Ensure buffer has the correct size
  if (callbackData.buffer.size() != static_cast<size_t>(fftSize)) {
    callbackData.buffer.resize(fftSize, 0.0f);
  }

  // Perform FFT on current buffer
  FFTResult fft = performFFT(callbackData.buffer);

  // Calculate frequency bands
  calculateFrequencyBands(fft, vizData);

  // Calculate RMS energy
  double sum = 0.0;
  for (const auto& sample : callbackData.buffer) {
    sum += sample * sample;
  }
  vizData.rmsEnergy = std::sqrt(sum / callbackData.buffer.size());

  // Calculate spectral centroid
  double weightedSum = 0.0;
  double magnitudeSum = 0.0;
  for (size_t i = 0; i < fft.magnitudes.size(); ++i) {
    weightedSum += fft.frequencies[i] * fft.magnitudes[i];
    magnitudeSum += fft.magnitudes[i];
  }
  vizData.spectralCentroid =
      (magnitudeSum > 0) ? weightedSum / magnitudeSum : 0.0;

  // Calculate spectral rolloff (frequency below which 85% of energy is
  // contained)
  double totalEnergy = 0.0;
  for (const auto& mag : fft.magnitudes) {
    totalEnergy += mag * mag;
  }

  double targetEnergy = 0.85 * totalEnergy;
  double currentEnergy = 0.0;
  vizData.spectralRolloff = 0.0;

  for (size_t i = 0; i < fft.magnitudes.size(); ++i) {
    currentEnergy += fft.magnitudes[i] * fft.magnitudes[i];
    if (currentEnergy >= targetEnergy) {
      vizData.spectralRolloff = fft.frequencies[i];
      break;
    }
  }

  // Update peak detection
  if (!callbackData.buffer.empty()) {
    vizData.currentPeak = *std::max_element(callbackData.buffer.begin(),
                                            callbackData.buffer.end());
  } else {
    vizData.currentPeak = 0.0;
  }
  updatePeakHistory(vizData);

  // Detect beats
  detectBeat(vizData);
}

void AudioSystem::calculateFrequencyBands(const FFTResult& fft,
                                          VisualizationData& data) {
  // Define frequency bands
  const double bassFreq = 250.0;
  const double lowMidFreq = 500.0;
  const double midFreq = 2000.0;
  const double highMidFreq = 4000.0;

  double bassSum = 0.0, lowMidSum = 0.0, midSum = 0.0, highMidSum = 0.0,
         trebleSum = 0.0;
  int bassCount = 0, lowMidCount = 0, midCount = 0, highMidCount = 0,
      trebleCount = 0;

  for (size_t i = 0; i < fft.frequencies.size(); ++i) {
    double freq = fft.frequencies[i];
    double magnitude = fft.magnitudes[i];

    if (freq <= bassFreq) {
      bassSum += magnitude;
      bassCount++;
    } else if (freq <= lowMidFreq) {
      lowMidSum += magnitude;
      lowMidCount++;
    } else if (freq <= midFreq) {
      midSum += magnitude;
      midCount++;
    } else if (freq <= highMidFreq) {
      highMidSum += magnitude;
      highMidCount++;
    } else {
      trebleSum += magnitude;
      trebleCount++;
    }
  }

  data.bassLevel = (bassCount > 0) ? bassSum / bassCount : 0.0;
  data.lowMidLevel = (lowMidCount > 0) ? lowMidSum / lowMidCount : 0.0;
  data.midLevel = (midCount > 0) ? midSum / midCount : 0.0;
  data.highMidLevel = (highMidCount > 0) ? highMidSum / highMidCount : 0.0;
  data.trebleLevel = (trebleCount > 0) ? trebleSum / trebleCount : 0.0;
}

void AudioSystem::detectBeat(VisualizationData& data) {
  // Simple beat detection based on energy threshold
  static double lastEnergy = 0.0;
  static double energyThreshold = 0.1;

  double energyChange = data.rmsEnergy - lastEnergy;

  if (energyChange > energyThreshold && data.rmsEnergy > 0.05) {
    data.isBeat = true;
    data.beatIntensity = std::min(energyChange / energyThreshold, 1.0);
  } else {
    data.isBeat = false;
    data.beatIntensity = 0.0;
  }

  lastEnergy = data.rmsEnergy;

  // Adaptive threshold
  energyThreshold = 0.1 + 0.5 * data.averagePeak;
}

void AudioSystem::updatePeakHistory(VisualizationData& data) {
  data.peakHistory.push_back(data.currentPeak);
  data.energyHistory.push_back(data.rmsEnergy);

  // Keep only last 30 samples (about 0.7 seconds at 44.1kHz)
  if (data.peakHistory.size() > 30) {
    data.peakHistory.pop_front();
  }
  if (data.energyHistory.size() > 30) {
    data.energyHistory.pop_front();
  }

  // Calculate average peak
  if (!data.peakHistory.empty()) {
    data.averagePeak =
        std::accumulate(data.peakHistory.begin(), data.peakHistory.end(), 0.0) /
        data.peakHistory.size();
  }
}

void AudioSystem::setFFTSize(int size) {
  if (size != fftSize && (size & (size - 1)) == 0) {  // Check if power of 2
    cleanupFFT();
    fftSize = size;
    initializeFFT();
    callbackData.bufferSize = size;
    callbackData.buffer.resize(size);
  }
}

void AudioSystem::audioCallback(void* userdata, Uint8* stream, int len) {
  // This callback is not used in the current implementation
  // since we're using a simulated approach for audio playback
  (void)userdata;
  (void)stream;
  (void)len;
}
