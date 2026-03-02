//
//  GrainScheduler.h
//  MicoForge
//
//  Created by Ben on 11.11.25.
//

#pragma once

#include "../CircularBuffer.h"
#include "../Helper.h"
#include "Grain.h"
#include "GrainEnvelope.h"
#include <array>
#include <juce_core/juce_core.h>
#include <stdio.h>

using namespace HelperFunctions;

/**
 * Granulation mode controlling how grain triggers are distributed over time.
 * Asynchronous: trigger intervals are randomised around the mean interval.
 * Synchronous:  grains are triggered at a fixed, periodic interval.
 */
enum class GranulationMode
{
    Asynchronous,
    Synchronous
};

/**
 * Selectable grain envelope shapes.
 * The integer values map directly to the ENVELOPE parameter choice index.
 */
enum class EnvelopeType
{
    Gaussian = 0,
    Parabolic = 1
};

/**
 * GrainScheduler manages a fixed-size pool of Grain objects.
 *
 * Each audio sample it checks whether a new grain should be triggered
 * (processTriggers), then sums the output of all active grains for each
 * channel (sumGrainsForChannel), and finally advances every grain by one
 * sample (advanceGrains).  This split allows the outer sample loop in
 * processBlock to interleave channel processing without redundant state
 * updates.
 *
 * The pool size of 128 grains is chosen to support high density values
 * (up to 128 grains/s × max grain duration) without dynamic allocation.
 */
class GrainScheduler
{
public:
    GrainScheduler();

    /**
     * Initialises the scheduler for the current audio session.
     * Must be called from prepareToPlay before any audio processing.
     * @param sampleRate Host sample rate in Hz
     */
    void prepare(int sampleRate);

    /**
     * Sets the target grain density (grains per second).
     * Recalculates the trigger interval after the update.
     * @param density Grains per second (1–128)
     */
    void setDensity(int density);

    /**
     * Sets the amount of random variation applied to the trigger interval.
     * @param densityVar Variation factor (0.0 = none, 1.0 = full)
     */
    void setDensityVar(float densityVar);

    /**
     * Sets the maximum scatter of the grain read position around the write head.
     * @param distribution Scatter range in samples
     */
    void setDistribution(float distribution);

    /**
     * Sets the base grain duration.
     * @param durationMs Duration in milliseconds
     */
    void setGrainDuration(float durationMs);

    /**
     * Sets the amount of random variation applied to grain duration.
     * @param grainDurationVar Variation factor (0.0 = none, 1.0 = full)
     */
    void setGrainDurationVar(float grainDurationVar);

    /**
     * Sets the base playback rate for all grains.
     * @param playbackRate Speed multiplier (1.0 = normal, 0.5 = half speed)
     */
    void setPlaybackRate(float playbackRate);

    /**
     * Sets the amount of random variation applied to the playback rate.
     * @param playbackRateVar Variation factor (0.0 = none)
     */
    void setPlaybackRateVar(float playbackRateVar);

    /**
     * Switches between asynchronous and synchronous granulation mode.
     * @param mode GranulationMode::Asynchronous or Synchronous
     */
    void setMode(GranulationMode mode);

    /**
     * Selects the grain envelope shape used for newly triggered grains.
     * @param type EnvelopeType enum value
     */
    void setEnvelopeType(EnvelopeType type);

    /** Recalculates triggerIntervalSamples_ from the current density. */
    void updateTimingFromDensity();

    /** Recalculates triggerIntervalSamples_ from density and sampleRate_. */
    void calculateTriggerIntervalSamples();

    /**
     * Checks whether a new grain should be triggered this sample and,
     * if so, activates an inactive grain from the pool.
     * Called once per sample from the outer sample loop in processBlock.
     * @param circularBuffer Input buffer used to determine the grain start position
     */
    void processTriggers(CircularBuffer& circularBuffer);

    /**
     * Accumulates the output of all active grains for one channel.
     * Called once per channel per sample after processTriggers.
     * @param circularBuffer Source buffer read by each active grain
     * @param channel        Output channel index
     * @return Sum of all active grain outputs for this channel
     */
    float sumGrainsForChannel(CircularBuffer& circularBuffer, int channel);

    /**
     * Advances all active grains by one sample.
     * Must be called exactly once per sample after all channels have been mixed.
     */
    void advanceGrains();

private:
    std::array<Grain, 128> grainPool; // fixed-size grain pool, no heap allocation in audio thread

    // timing
    int sampleCounter_ = 0; // samples elapsed since the last grain trigger
    int grainDurationSamples_ = 0;
    float grainDurationVar_;
    // average interval between the trigger of grains
    float triggerIntervalSamples_ = 0;
    int sampleRate_;

    // number of grains per second
    int density_;
    float densityVar_;
    float distributionSamples_; // maximum scatter of the read position in samples
    float grainPlaybackRate_;
    float grainPlaybackRateVar_;

    float grainAmplitude_;

    // mode and randomization
    enum GranulationMode mode_;
    juce::Random random_;

    const GrainEnvelope* currentEnvelope_; // non-owning pointer, updated by setEnvelopeType
    GaussianEnvelope gaussianEnvelope_;
    ParabolicEnvelope parabolicEnvelope_;
};
