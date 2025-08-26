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
      wavData(nullptr),
      wavDataLen(0),
      playing(false),
      paused(false),
      currentSample(0),
      playbackStartTime(0) {
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

  // Initialize FFT buffer for visualization
  fftBuffer.resize(fftSize, 0.0f);

  std::cout << "AudioSystem: Initialization complete" << std::endl;
}

AudioSystem::~AudioSystem() {
  std::cout << "AudioSystem: Destructing..." << std::endl;
  stopPlayback();
  cleanupFFT();

  // Free WAV data
  if (wavData) {
    SDL_free(wavData);
    wavData = nullptr;
    wavDataLen = 0;
  }

  // Ensure audio stream is destroyed
  if (audioStream) {
    SDL_DestroyAudioStream(audioStream);
    audioStream = nullptr;
  }
}

bool AudioSystem::loadAudioFile(const std::string& filename) {
  std::cout << "AudioSystem: Loading audio file: " << filename << std::endl;

  // Stop any current playback
  stopPlayback();

  // Free any existing WAV data
  if (wavData) {
    SDL_free(wavData);
    wavData = nullptr;
    wavDataLen = 0;
  }

  // Check file extension
  std::string extension = filename.substr(filename.find_last_of(".") + 1);
  std::transform(extension.begin(), extension.end(), extension.begin(),
                 ::tolower);

  if (extension != "wav") {
    std::cerr << "Unsupported file format: " << extension
              << ". Only WAV files are supported." << std::endl;
    return false;
  }

  SDL_AudioSpec spec;

  std::cout << "AudioSystem: About to call SDL_LoadWAV..." << std::endl;
  if (!SDL_LoadWAV(filename.c_str(), &spec, &wavData, &wavDataLen)) {
    std::cerr << "Failed to load WAV file: " << SDL_GetError() << std::endl;
    return false;
  }

  std::cout << "AudioSystem: WAV loaded successfully" << std::endl;
  std::cout << "  Sample rate: " << spec.freq << std::endl;
  std::cout << "  Channels: " << spec.channels << std::endl;
  std::cout << "  Format: " << spec.format << std::endl;
  std::cout << "  Audio length: " << wavDataLen << " bytes" << std::endl;

  // Store original audio format
  audioData.sampleRate = spec.freq;
  audioData.channels = spec.channels;
  audioData.totalSamples =
      wavDataLen / (spec.channels * SDL_AUDIO_BYTESIZE(spec.format));

  std::cout << "AudioSystem: Total samples: " << audioData.totalSamples
            << std::endl;

  // Convert WAV data to our internal format for FFT analysis
  // For visualization, we'll use only the first channel if stereo
  size_t samplesPerChannel = audioData.totalSamples / spec.channels;
  audioData.samples.resize(samplesPerChannel);

  // Convert based on format
  if (spec.format == SDL_AUDIO_S16) {
    Sint16* samples = reinterpret_cast<Sint16*>(wavData);
    for (size_t i = 0; i < samplesPerChannel; ++i) {
      // For stereo, average the channels; for mono, just use the sample
      if (spec.channels == 2) {
        audioData.samples[i] = (samples[i * 2] + samples[i * 2 + 1]) / 2;
      } else {
        audioData.samples[i] = samples[i];
      }
    }
  } else if (spec.format == SDL_AUDIO_S32) {
    Sint32* samples = reinterpret_cast<Sint32*>(wavData);
    for (size_t i = 0; i < samplesPerChannel; ++i) {
      if (spec.channels == 2) {
        audioData.samples[i] = static_cast<Sint16>(
            (samples[i * 2] + samples[i * 2 + 1]) / 2 >> 16);
      } else {
        audioData.samples[i] = static_cast<Sint16>(samples[i] >> 16);
      }
    }
  } else if (spec.format == SDL_AUDIO_F32) {
    float* samples = reinterpret_cast<float*>(wavData);
    for (size_t i = 0; i < samplesPerChannel; ++i) {
      if (spec.channels == 2) {
        audioData.samples[i] = static_cast<Sint16>(
            (samples[i * 2] + samples[i * 2 + 1]) / 2.0f * 32767.0f);
      } else {
        audioData.samples[i] = static_cast<Sint16>(samples[i] * 32767.0f);
      }
    }
  } else {
    std::cerr << "Unsupported audio format: " << spec.format << std::endl;
    SDL_free(wavData);
    wavData = nullptr;
    wavDataLen = 0;
    return false;
  }

  // Update total samples to reflect the mono conversion
  audioData.totalSamples = samplesPerChannel;
  audioData.channels = 1;  // We're now working with mono data for visualization

  // Store the original spec for playback
  originalSpec = spec;

  audioData.loaded = true;

  std::cout << "AudioSystem: Audio file loaded successfully" << std::endl;
  std::cout << "  Samples: " << audioData.totalSamples
            << ", Rate: " << audioData.sampleRate
            << ", Channels: " << audioData.channels << std::endl;

  // Debug: Check first few samples
  std::cout << "AudioSystem: First 10 samples: ";
  for (int i = 0; i < std::min(10, static_cast<int>(audioData.samples.size()));
       ++i) {
    std::cout << audioData.samples[i] << " ";
  }
  std::cout << std::endl;

  return true;
}

void AudioSystem::startPlayback() {
  if (!audioData.loaded || playing) return;

  // Initialize SDL audio if not already done
  if (SDL_WasInit(SDL_INIT_AUDIO) == 0) {
    if (!SDL_InitSubSystem(SDL_INIT_AUDIO)) {
      std::cerr << "Failed to initialize SDL audio: " << SDL_GetError()
                << std::endl;
      return;
    }
  }

  // Create audio stream using the original WAV format
  audioStream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
                                          &originalSpec, NULL, NULL);

  if (!audioStream) {
    std::cerr << "Failed to create audio stream: " << SDL_GetError()
              << std::endl;
    return;
  }

  // Start playback
  currentSample = 0;
  playbackStartTime = SDL_GetTicks();
  playing = true;
  paused = false;

  // Resume the audio stream (SDL3 opens streams in paused state)
  SDL_ResumeAudioStreamDevice(audioStream);

  std::cout << "Audio playback started" << std::endl;
  std::cout << "  Sample rate: " << originalSpec.freq << std::endl;
  std::cout << "  Channels: " << originalSpec.channels << std::endl;
  std::cout << "  Format: " << originalSpec.format << std::endl;
}

void AudioSystem::stopPlayback() {
  if (playing) {
    playing = false;
    paused = false;
    currentSample = 0;

    // Destroy the audio stream
    if (audioStream) {
      SDL_DestroyAudioStream(audioStream);
      audioStream = nullptr;
    }

    std::cout << "Audio playback stopped" << std::endl;
  }
}

void AudioSystem::pausePlayback() {
  if (playing && !paused) {
    paused = true;
    if (audioStream) {
      SDL_PauseAudioStreamDevice(audioStream);
    }
    std::cout << "Audio playback paused" << std::endl;
  } else if (playing && paused) {
    paused = false;
    if (audioStream) {
      SDL_ResumeAudioStreamDevice(audioStream);
    }
    std::cout << "Audio playback resumed" << std::endl;
  }
}

double AudioSystem::getPlaybackPosition() const {
  if (!audioData.loaded || !playing) return 0.0;

  Uint64 currentTime = SDL_GetTicks();
  if (playbackStartTime == 0) return 0.0;

  Uint64 elapsedMs = currentTime - playbackStartTime;
  double elapsedSeconds = elapsedMs / 1000.0;
  return elapsedSeconds;
}

double AudioSystem::getDuration() const {
  if (!audioData.loaded) return 0.0;
  return static_cast<double>(audioData.totalSamples) / audioData.sampleRate;
}

void AudioSystem::updatePlayback() {
  if (!playing || !audioStream || !wavData) return;

  // Check if we need to feed the audio stream more data
  // We're being lazy here, but if there's less than the entire wav file left to
  // play, just shove a whole copy of it into the queue, so we always have tons
  // of data queued for playback
  if (SDL_GetAudioStreamQueued(audioStream) < (int)wavDataLen) {
    // Feed more data to the stream. It will queue at the end, and trickle out
    // as the hardware needs more data
    SDL_PutAudioStreamData(audioStream, wavData, wavDataLen);

    // Update current sample position for visualization
    currentSample =
        (currentSample + audioData.totalSamples) % audioData.totalSamples;
  }
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
  if (fftBuffer.empty()) {
    fftBuffer.resize(fftSize, 0.0f);
  }

  if (!playing || !audioData.loaded) {
    // If not playing or no audio loaded, just return with zero data
    std::fill(fftBuffer.begin(), fftBuffer.end(), 0.0f);
    return;
  }

  // Calculate the actual current playback position based on elapsed time
  Uint64 currentTime = SDL_GetTicks();
  if (playbackStartTime == 0) {
    playbackStartTime = currentTime;
  }

  Uint64 elapsedMs = currentTime - playbackStartTime;
  double elapsedSeconds = elapsedMs / 1000.0;
  size_t actualSamplePosition =
      static_cast<size_t>(elapsedSeconds * audioData.sampleRate) %
      audioData.totalSamples;

  // Get current audio data for visualization using the actual playback position
  size_t startSample = actualSamplePosition;
  size_t numSamples = std::min(static_cast<size_t>(fftSize),
                               audioData.samples.size() - startSample);

  if (numSamples > 0) {
    // Convert Sint16 samples to normalized float values for FFT
    fftBuffer.clear();
    fftBuffer.reserve(fftSize);

    for (size_t i = 0; i < numSamples; ++i) {
      Sint16 sample = audioData.samples[startSample + i];
      float normalizedSample = static_cast<float>(sample) / 32767.0f;
      fftBuffer.push_back(normalizedSample);
    }

    // Pad with zeros if needed
    while (fftBuffer.size() < static_cast<size_t>(fftSize)) {
      fftBuffer.push_back(0.0f);
    }
  } else {
    // No samples available, fill with zeros
    std::fill(fftBuffer.begin(), fftBuffer.end(), 0.0f);
  }

  // Ensure buffer has the correct size
  if (fftBuffer.size() != static_cast<size_t>(fftSize)) {
    fftBuffer.resize(fftSize, 0.0f);
  }

  // Perform FFT on current buffer
  FFTResult fft = performFFT(fftBuffer);

  // Calculate frequency bands
  calculateFrequencyBands(fft, vizData);

  // Calculate RMS energy
  double sum = 0.0;
  for (const auto& sample : fftBuffer) {
    sum += sample * sample;
  }
  vizData.rmsEnergy = std::sqrt(sum / fftBuffer.size());

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
  if (!fftBuffer.empty()) {
    vizData.currentPeak = *std::max_element(fftBuffer.begin(), fftBuffer.end());
  } else {
    vizData.currentPeak = 0.0;
  }
  updatePeakHistory(vizData);

  // Detect beats
  detectBeat(vizData);

  // Debug output (remove this later)
  static int debugCounter = 0;
  if (++debugCounter % 60 ==
      0) {  // Print every 60 frames (about once per second)
    std::cout << "AudioSystem: Visualization update - "
              << "Playing: " << playing << ", RMS Energy: " << vizData.rmsEnergy
              << ", Current Peak: " << vizData.currentPeak
              << ", Bass Level: " << vizData.bassLevel
              << ", Sample Position: " << actualSamplePosition
              << ", FFT Buffer Size: " << fftBuffer.size()
              << ", Max FFT Magnitude: "
              << (fft.magnitudes.empty()
                      ? 0.0
                      : *std::max_element(fft.magnitudes.begin(),
                                          fft.magnitudes.end()))
              << std::endl;
  }
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
    fftBuffer.resize(size);
  }
}
