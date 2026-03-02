/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginEditor.h"
#include "PluginProcessor.h"
#include "granular-processor/Grain.h"
#include "granular-processor/GrainScheduler.h"

//==============================================================================
MicroForgeAudioProcessor::MicroForgeAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
                         )
    , apvts(*this, nullptr, "Parameters", createParameters())
    , circularBuffer_{}
    , delayBuffer_{}
#endif
{
}

MicroForgeAudioProcessor::~MicroForgeAudioProcessor() {}

//==============================================================================
const juce::String MicroForgeAudioProcessor::getName() const { return JucePlugin_Name; }

bool MicroForgeAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool MicroForgeAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool MicroForgeAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double MicroForgeAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int MicroForgeAudioProcessor::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
              // so this should be at least 1, even if you're not really implementing programs.
}

int MicroForgeAudioProcessor::getCurrentProgram() { return 0; }

void MicroForgeAudioProcessor::setCurrentProgram(int index) { juce::ignoreUnused(index); }

const juce::String MicroForgeAudioProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);

    return {};
}

void MicroForgeAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

//==============================================================================
void MicroForgeAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused(samplesPerBlock);

    auto totalNumOutputChannels = getTotalNumOutputChannels();
    auto circularBufferSize = static_cast<int>(sampleRate * 5.0);

    circularBuffer_ = CircularBuffer(totalNumOutputChannels, circularBufferSize);
    delayBuffer_ = CircularBuffer(totalNumOutputChannels, circularBufferSize);

    // Initialize SmoothedValue objects
    smoothedDensity_.reset(sampleRate, 0.1);
    smoothedDensityVar_.reset(sampleRate, 0.1);
    smoothedDistribution_.reset(sampleRate, 0.1);
    smoothedGrainLength_.reset(sampleRate, 0.3); // 300ms ramp time
    smoothedGrainLengthVar_.reset(sampleRate, 0.3);
    smoothedDelayTime_.reset(sampleRate, 0.1);
    smoothedDelayTimeVar_.reset(sampleRate, 0.1);
    smoothedDelayFeedback_.reset(sampleRate, 0.1);
    smoothedPlaybackRate_.reset(sampleRate, 0.3);
    smoothedPlaybackRateVar_.reset(sampleRate, 0.3);
    smoothedMix_.reset(sampleRate, 0.05);

    // Set initial values
    smoothedDensity_.setCurrentAndTargetValue(apvts.getRawParameterValue("DENSITY")->load());
    smoothedDensityVar_.setCurrentAndTargetValue(apvts.getRawParameterValue("DENSITY VAR")->load());
    smoothedDistribution_.setCurrentAndTargetValue(
        apvts.getRawParameterValue("DISTRIBUTION")->load());
    smoothedGrainLength_.setCurrentAndTargetValue(
        apvts.getRawParameterValue("GRAIN LENGTH")->load());
    smoothedGrainLengthVar_.setCurrentAndTargetValue(
        apvts.getRawParameterValue("GRAIN LENGTH VAR")->load());
    smoothedDelayTime_.setCurrentAndTargetValue(apvts.getRawParameterValue("DELAY TIME")->load());
    smoothedDelayTimeVar_.setCurrentAndTargetValue(
        apvts.getRawParameterValue("DELAY TIME VAR")->load());
    smoothedDelayFeedback_.setCurrentAndTargetValue(
        apvts.getRawParameterValue("DELAY FEEDBACK")->load());
    smoothedPlaybackRate_.setCurrentAndTargetValue(
        apvts.getRawParameterValue("PLAYBACK RATE")->load());
    smoothedPlaybackRateVar_.setCurrentAndTargetValue(
        apvts.getRawParameterValue("PLAYBACK RATE VAR")->load());
    smoothedMix_.setCurrentAndTargetValue(apvts.getRawParameterValue("MIX")->load());

    grainScheduler_.prepare(static_cast<int>(sampleRate));
    grainScheduler_.setGrainDuration(50.0f);
    grainScheduler_.setDensity(5);
}

void MicroForgeAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MicroForgeAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
        layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void MicroForgeAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                            juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Update smoothed parameters
    smoothedDensity_.setTargetValue(apvts.getRawParameterValue("DENSITY")->load());
    smoothedDensityVar_.setTargetValue(apvts.getRawParameterValue("DENSITY VAR")->load());
    smoothedDistribution_.setTargetValue(apvts.getRawParameterValue("DISTRIBUTION")->load());
    smoothedGrainLength_.setTargetValue(apvts.getRawParameterValue("GRAIN LENGTH")->load());
    smoothedGrainLengthVar_.setTargetValue(apvts.getRawParameterValue("GRAIN LENGTH VAR")->load());
    smoothedDelayTime_.setTargetValue(apvts.getRawParameterValue("DELAY TIME")->load());
    smoothedDelayTimeVar_.setTargetValue(apvts.getRawParameterValue("DELAY TIME VAR")->load());
    smoothedDelayFeedback_.setTargetValue(apvts.getRawParameterValue("DELAY FEEDBACK")->load());
    smoothedPlaybackRate_.setTargetValue(apvts.getRawParameterValue("PLAYBACK RATE")->load());
    smoothedPlaybackRateVar_.setTargetValue(
        apvts.getRawParameterValue("PLAYBACK RATE VAR")->load());
    smoothedMix_.setTargetValue(apvts.getRawParameterValue("MIX")->load());

    // Update grain scheduler parameters
    // grainScheduler_.setGrainDuration(300);
    // grainScheduler_.setDensity(40);
    // Set mode
    // grainScheduler_.setMode(GranulationMode::Asynchronous);

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // Get bool parameter of button first and convert to enum
    bool isAsynchronous = apvts.getRawParameterValue("MODE")->load() > 0.5f;
    grainScheduler_.setMode(isAsynchronous ? GranulationMode::Asynchronous
                                           : GranulationMode::Synchronous);
    // Get envelope type selection index and convert it to EnvelopeType
    int envelopeIndex = static_cast<int>(apvts.getRawParameterValue("ENVELOPE")->load());
    grainScheduler_.setEnvelopeType(static_cast<EnvelopeType>(envelopeIndex));

    //    for (int channel = 0; channel < juce::jmin(totalNumInputChannels, totalNumOutputChannels);
    //         ++channel)
    //    {
    //        // pointer to current channel array of samples
    //        auto* channelData = buffer.getWritePointer(channel);
    //
    //        circularBuffer_.fillBuffer(buffer, channel);
    //
    //        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    //        {
    //            // Save dry signal
    //            float drySignal = channelData[sample];
    //
    //            // Update grain scheduler parameters with smoothed values
    //            grainScheduler_.setGrainDuration(smoothedGrainLength_.getNextValue());
    //            grainScheduler_.setGrainDurationVar(smoothedGrainLengthVar_.getNextValue());
    //            grainScheduler_.setDelayTime(smoothedDelayTime_.getNextValue());
    //            grainScheduler_.setDelayTimeVar(smoothedDelayTimeVar_.getNextValue());
    //            grainScheduler_.setDensity(static_cast<int>(smoothedDensity_.getNextValue()));
    //            grainScheduler_.setDensityVar(smoothedDensityVar_.getNextValue());
    //            grainScheduler_.setPlaybackRate(smoothedPlaybackRate_.getNextValue());
    //
    //            // Process wet signal
    //            float wetSignal = grainScheduler_.processNextSample(circularBuffer_, channel);
    //            // Mix dry / wet based on MIX parameter
    //            float mixValue = smoothedMix_.getNextValue();
    //            channelData[sample] = drySignal * (1.0f - mixValue) + wetSignal * mixValue;
    //        }
    //    }
    //    circularBuffer_.updateBufferPositions(buffer);

    // fill circular buffer for all channels before the sample loop
    for (int channel = 0; channel < juce::jmin(totalNumInputChannels, totalNumOutputChannels);
         ++channel)
    {
        circularBuffer_.fillBuffer(buffer, channel);
    }

    // calculate numChannels once to avoid repeating jmin every sample
    int numChannels = juce::jmin(totalNumInputChannels, totalNumOutputChannels);

    // outer loop over samples so grain triggers and advances happen once per sample,
    // not once per channel — avoids double-advancing grains in stereo
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        // push smoothed parameter values into the scheduler for this sample
        grainScheduler_.setDensity(static_cast<int>(smoothedDensity_.getNextValue()));
        grainScheduler_.setDensityVar(smoothedDensityVar_.getNextValue());
        grainScheduler_.setDistribution(smoothedDistribution_.getNextValue());
        grainScheduler_.setGrainDuration(smoothedGrainLength_.getNextValue());
        grainScheduler_.setGrainDurationVar(smoothedGrainLengthVar_.getNextValue());
        //        grainScheduler_.setDelayTime(smoothedDelayTime_.getNextValue());
        //        grainScheduler_.setDelayTimeVar(smoothedDelayTimeVar_.getNextValue());
        grainScheduler_.setPlaybackRate(smoothedPlaybackRate_.getNextValue());
        grainScheduler_.setPlaybackRateVar(smoothedPlaybackRateVar_.getNextValue());

        // check whether a new grain should be triggered this sample
        grainScheduler_.processTriggers(circularBuffer_);

        float sampleRate = static_cast<float>(this->getSampleRate());

        float mixValue = smoothedMix_.getNextValue();
        float delayTimeMs = smoothedDelayTime_.getNextValue();
        float delayTimeVar = smoothedDelayTimeVar_.getNextValue();
        float feedback = smoothedDelayFeedback_.getNextValue();
        // convert delay time from ms to samples and apply stochastic variation
        float delaySamples = HelperFunctions::applyVariation(
            random_, delayTimeMs * 0.001f * sampleRate, delayTimeVar,
            HelperFunctions::DistributionType::Gaussian);
        delaySamples = juce::jmax(
            1.0f,
            delaySamples); // clamp to at least 1 sample to avoid reading the current write position

        // inner loop over channels — reads the same grain state for each channel
        for (int channel = 0; channel < numChannels; ++channel)
        {
            auto* channelData = buffer.getWritePointer(channel);
            float drySignal = channelData[sample];

            // sum all active grains for this channel
            float granulatedSignal = grainScheduler_.sumGrainsForChannel(circularBuffer_, channel);

            // read the delayed sample from before the current write head
            float readPosition = static_cast<float>(delayBuffer_.getWritePosition()) - delaySamples;
            float delayedSample = delayBuffer_.getSample(channel, readPosition);

            // write granulated signal mixed with feedback into the delay buffer
            delayBuffer_.writeSample(channel, granulatedSignal + delayedSample * feedback);

            // blend dry and wet (delayed granulated) signal
            channelData[sample] = drySignal * (1.0f - mixValue) + delayedSample * mixValue;
        }
        // advance the delay buffer write head once after all channels are written
        delayBuffer_.advanceWritePosition();
        // advance all grain envelope and read positions by one sample
        grainScheduler_.advanceGrains();
    }
    circularBuffer_.updateBufferPositions(buffer);
}

//==============================================================================
bool MicroForgeAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MicroForgeAudioProcessor::createEditor()
{
    return new MicroForgeAudioProcessorEditor(*this);
}

//==============================================================================
void MicroForgeAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void MicroForgeAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new MicroForgeAudioProcessor(); }

juce::AudioProcessorValueTreeState::ParameterLayout MicroForgeAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // UI parameters
    params.push_back(
        std::make_unique<juce::AudioParameterFloat>("DENSITY", "Density", 1.0f, 128.0f, 10.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("DENSITY VAR", "Density Variation",
                                                                 0.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("DISTRIBUTION", "Distribution",
                                                                 0.0f, 100.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("GRAIN LENGTH", "Grain Length",
                                                                 1.0f, 200.0f, 50.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "GRAIN LENGTH VAR", "Grain Length Variation", 0.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("DELAY TIME", "Delay Time", 10.0f,
                                                                 2000.0f, 500.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "DELAY TIME VAR", "Delay Time Variation", 0.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("DELAY FEEDBACK", "Delay Feedback",
                                                                 0.0f, 0.95f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "PLAYBACK RATE", "Delay Time Variation", 0.2f, 2.0f, 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "PLAYBACK RATE VAR", "Delay Time Variation", 0.0f, 3.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("MIX", "Mix", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterBool>("MODE", "Mode", true));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        "ENVELOPE", "Envelope Type", juce::StringArray{"Gaussian", "Parabolic"}, 0));

    return {params.begin(), params.end()};
}
