#pragma once

namespace tremolo {
class Tremolo {
public:
  enum class LfoWaveform : size_t {
    sine = 0,
    triangle = 1,
  };
  Tremolo() {
    for (auto& lfo : lfos) {
      lfo.setFrequency(5.f /* Hz */, true);
    }
  }
  void prepare(double sampleRate, int expectedMaxFramesPerBlock) {
    const juce::dsp::ProcessSpec processSpec {
      .sampleRate = sampleRate,
      .maximumBlockSize = static_cast<juce::uint32>(expectedMaxFramesPerBlock),
      .numChannels = 1u,
    };
    for (auto& lfo : lfos) {
      lfo.prepare(processSpec);
    }

    lfoTransitionSmoother.reset(sampleRate, 0.025f);

  }

  void setLfoWaveform(LfoWaveform waveform) {
    jassert(waveform == LfoWaveform::sine || waveform == LfoWaveform::triangle);
    lfoToSet = waveform;
  }

  void setModulationRate(float rateHz) {
    for (auto& lfo : lfos) {
      lfo.setFrequency(rateHz);
    }
  }

  void process(juce::AudioBuffer<float>& buffer) noexcept {
    updateLfoWaveform();
    // for each frame
    for (const auto frameIndex : std::views::iota(0, buffer.getNumSamples())) {
      // generate the LFO value
      const auto lfoValue = getNextLfoValue();
      // calculate the modulation value
      constexpr auto modulationDepth = 0.4f;
      // const auto modulationValue = modulationDepth * lfoValue + 1.f; Simplified, would allow output signal to have higher amplitude than input sometimes
      const auto modulationValue = (1.f - modulationDepth) + modulationDepth * (0.5f * (lfoValue + 1.f));

      // for each channel sample in the frame
      for (const auto channelIndex :
           std::views::iota(0, buffer.getNumChannels())) {
        // get the input sample
        const auto inputSample = buffer.getSample(channelIndex, frameIndex);

        // modulate the sample
        const auto outputSample = inputSample * modulationValue;

        // set the output sample
        buffer.setSample(channelIndex, frameIndex, outputSample);
      }
    }
  }

  void reset() noexcept {
    for (auto& lfo : lfos) {
      lfo.reset();
    }
  }

private:
  // You should put class members and private functions here
  float getNextLfoValue() {
    if (lfoTransitionSmoother.isSmoothing()) {
      return lfoTransitionSmoother.getNextValue();
    }
    return lfos[juce::toUnderlyingType(currentLfo)].processSample(0.f);
  }
  static float triangle(float phase) {
    const auto ft = phase / juce::MathConstants<float>::twoPi;
    return 4.f * std::abs(phase - std::floor(ft + 0.5f)) - 1.f;
  }

  void updateLfoWaveform() {
    if (currentLfo != lfoToSet) {
      lfoTransitionSmoother.setCurrentAndTargetValue(getNextLfoValue());
      currentLfo = lfoToSet;
      lfoTransitionSmoother.setTargetValue(getNextLfoValue());
    }
  }

  std::array<juce::dsp::Oscillator<float>, 2> lfos{
    juce::dsp::Oscillator<float>{[](float phase) { return std::sin(phase); }},
    juce::dsp::Oscillator<float>{[](float phase) { return (2.0f / juce::MathConstants<float>::pi) * std::asin(std::sin(phase)); }}
  };
  LfoWaveform currentLfo = LfoWaveform::sine;
  LfoWaveform lfoToSet = currentLfo;
  juce::SmoothedValue<float> lfoTransitionSmoother{0.f};
};
}  // namespace tremolo
