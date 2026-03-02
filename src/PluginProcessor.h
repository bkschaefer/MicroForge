/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

#include "CircularBuffer.h"
#include "granular-processor/GrainScheduler.h"

//==============================================================================
/**
 * Main audio processor for the MicroForge granular effect plugin.
 *
 * Implements a real-time granular delay:
 *  1. Incoming audio is continuously written into circularBuffer_.
 *  2. GrainScheduler reads short fragments (grains) from that buffer,
 *     applies an envelope, and outputs them as a granulated signal.
 *  3. The granulated signal is fed into a feedback delay line (delayBuffer_).
 *  4. Dry and wet signals are blended according to the MIX parameter.
 *
 * All continuously varying parameters are smoothed with SmoothedValue to
 * avoid audible discontinuities when the user moves a control.
 */
class MicroForgeAudioProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    MicroForgeAudioProcessor();
    ~MicroForgeAudioProcessor() override;

    //==============================================================================
    /**
     * Allocates and initialises buffers and SmoothedValue objects.
     * The circular buffer is sized to hold 5 seconds of audio.
     * @param sampleRate      Host sample rate in Hz
     * @param samplesPerBlock Expected block size (unused, buffers are per-sample)
     */
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    /**
     * Core audio callback — called by the host once per audio block.
     *
     * Processing order per block:
     *  1. Advance all SmoothedValues to their new targets.
     *  2. Fill circularBuffer_ with the current dry input.
     *  3. Per sample: update scheduler parameters, trigger grains,
     *     sum grain outputs per channel, apply feedback delay, mix dry/wet.
     *  4. Advance the circular buffer write position.
     *
     * @param buffer       Input/output audio block (modified in-place)
     * @param midiMessages Unused MIDI buffer
     */
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    /** Public APVTS — accessed by the editor to attach slider attachments. */
    juce::AudioProcessorValueTreeState apvts;

private:
    CircularBuffer circularBuffer_; // main input ring buffer (5 s)
    CircularBuffer delayBuffer_;    // feedback delay ring buffer (5 s)
    GrainScheduler grainScheduler_;
    juce::Random random_;

    // smoothed parameter values — prevent zipper noise on knob changes
    juce::SmoothedValue<float> smoothedDensity_;
    juce::SmoothedValue<float> smoothedDensityVar_;
    juce::SmoothedValue<float> smoothedDistribution_;
    juce::SmoothedValue<float> smoothedGrainLength_;
    juce::SmoothedValue<float> smoothedGrainLengthVar_;
    juce::SmoothedValue<float> smoothedDelayTime_;
    juce::SmoothedValue<float> smoothedDelayTimeVar_;
    juce::SmoothedValue<float> smoothedDelayFeedback_;
    juce::SmoothedValue<float> smoothedPlaybackRate_;
    juce::SmoothedValue<float> smoothedPlaybackRateVar_;
    juce::SmoothedValue<float> smoothedMix_;

    /** Defines all plugin parameters and their ranges. */
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MicroForgeAudioProcessor)
};
