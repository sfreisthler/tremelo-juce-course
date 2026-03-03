namespace tremolo {

namespace {
juce::AudioParameterFloat& createModulationRateParameter(
    juce::AudioProcessor& processor) {
  constexpr int versionHint = 1;

  auto parameter = std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"modulation.rate", versionHint}, "Modulation rate",
      juce::NormalisableRange<float>{0.1f, 20.0f, 0.01f, 0.4f}, 5.0f,
      juce::AudioParameterFloatAttributes{}.withLabel("Hz"));

  auto& parameterReference = *parameter;
  processor.addParameter(parameter.release());
  return parameterReference;
}
juce::AudioParameterFloat& createGainParameter(juce::AudioProcessor& processor) {
  constexpr int versionHint = 1;
  auto parameter = std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{"gain", versionHint}, "Output gain",
      juce::NormalisableRange<float>{-12.f, 12.f, 0.1f, 0.4f}, 0.f,
      juce::AudioParameterFloatAttributes{}.withLabel("dB"));

  auto& parameterReference = *parameter;
  processor.addParameter(parameter.release());
  return parameterReference;
}
}  // namespace

Parameters::Parameters(juce::AudioProcessor& processor)
    : rate{createModulationRateParameter(processor)},
      gain{createGainParameter(processor)} {}

}  // namespace tremolo
