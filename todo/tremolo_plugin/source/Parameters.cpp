namespace tremolo {

namespace {
juce::AudioParameterFloat& createModulationRateParameter(juce::AudioProcessor& processor)
{
    constexpr int versionHint = 1;

    auto parameter = std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ "modulation.rate", versionHint },
        "Modulation rate",
        juce::NormalisableRange<float>{ 0.1f, 20.0f, 0.01f, 0.4f },
        5.0f,
        juce::AudioParameterFloatAttributes{}.withLabel("Hz")
    );

    auto& parameterReference = *parameter;
    processor.addParameter(parameter.release());
    return parameterReference;
}
} // namespace

Parameters::Parameters(juce::AudioProcessor& processor)
    : rate{ createModulationRateParameter(processor) }
{
    // no need for ignoreUnused now, processor is used
}

} // namespace tremolo
