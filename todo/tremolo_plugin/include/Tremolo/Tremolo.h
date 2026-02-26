#pragma once
#include "juce_core/maths/juce_MathsFunctions.h"

namespace tremolo {
class Tremolo {
public:
  Tremolo() {
    lfo.setFrequency(5.f /* Hz */, true);
  }
  void prepare(double sampleRate, int expectedMaxFramesPerBlock) {
    const juce::dsp::ProcessSpec processSpec {
      .sampleRate = sampleRate,
      .maximumBlockSize = static_cast<juce::uint32>(expectedMaxFramesPerBlock),
      .numChannels = 1u,
    };
    lfo.prepare(processSpec);
  }

  void process(juce::AudioBuffer<float>& buffer) noexcept {
    // for each frame
    for (const auto frameIndex : std::views::iota(0, buffer.getNumSamples())) {
      // generate the LFO value
      const auto lfoValue = lfo.processSample(0.f); // we pass 0.f because oscillator class adds generated output to whatever we pass in, we just want generated value
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
    lfo.reset();
  }

private:
  // You should put class members and private functions here
  juce::dsp::Oscillator<float> lfo{ [](auto phase) {return std::sin(phase); }};
};
}  // namespace tremolo
