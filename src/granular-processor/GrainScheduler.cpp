//
//  GrainScheduler.cpp
//  MicoForge
//
//  Created by Ben on 11.11.25.
//

#include "../CircularBuffer.h"
#include "../Helper.h"
#include "GrainScheduler.h"

GrainScheduler::GrainScheduler()
    : sampleRate_(44100)
    , density_(10)
    , distributionSamples_(0.0f)
    , grainPlaybackRate_(1.0f)
    , grainAmplitude_(1.0f)
    , currentEnvelope_(&gaussianEnvelope_)
{
}

void GrainScheduler::prepare(int sampleRate)
{
    sampleRate_ = sampleRate;
    sampleCounter_ = 0;
    // updateTimingFromDensity();
    calculateTriggerIntervalSamples();
}

void GrainScheduler::setDensity(int density) { density_ = density; }

void GrainScheduler::setDensityVar(float densityVar) { densityVar_ = densityVar; }

void GrainScheduler::setDistribution(float distributionMs)
{
    distributionSamples_ = distributionMs * 0.001f * sampleRate_;
}

void GrainScheduler::setGrainDuration(float durationMs)
{
    // ms -> to seconds * sampleRate to get the sample for this timeframe
    grainDurationSamples_ = static_cast<int>(durationMs * 0.001f * sampleRate_);
}

void GrainScheduler::setGrainDurationVar(float grainDurationVar)
{
    grainDurationVar_ = grainDurationVar;
}

void GrainScheduler::setPlaybackRate(float playbackRate) { grainPlaybackRate_ = playbackRate; }

void GrainScheduler::setPlaybackRateVar(float playbackRateVar)
{
    grainPlaybackRateVar_ = playbackRateVar;
}

void GrainScheduler::setMode(GranulationMode mode) { mode_ = mode; }

void GrainScheduler::setEnvelopeType(EnvelopeType type)
{
    switch (type)
    {
        case EnvelopeType::Gaussian:
            currentEnvelope_ = &gaussianEnvelope_;
            break;
        case EnvelopeType::Parabolic:
            currentEnvelope_ = &parabolicEnvelope_;
            break;
    }
}

void GrainScheduler::updateTimingFromDensity()
{
    // handle edge case: density too low
    if (density_ <= 0)
    {
        return;
    }

    // calculate the interval
    int interval = static_cast<int>(sampleRate_ / density_);

    // handle edge case: interval too small (density too high)
    if (interval < 1)
    {
        interval = 1;
    }
    triggerIntervalSamples_ = interval;
}

/**
 * Calculate next trigger interval based on currently selected mode.
 * Asynchronous: uses Bencina's formula `-log(random) / density * sampleRate`
 * Synchronous: uses periodic formula sampleRate / density
 *
 */
void GrainScheduler::calculateTriggerIntervalSamples()
{
    // handle edge case if density is too low
    if (density_ <= 0)
    {
        triggerIntervalSamples_ = static_cast<float>(sampleRate_);
        return;
    }

    if (mode_ == GranulationMode::Asynchronous)
    {
        // Bencina's formula: -log(random) / density * sampleRate
        // returns value in range of 0.0 - 1.0
        float randomValue = random_.nextFloat();

        if (randomValue < 0.0001f)
        {
            randomValue = 0.0001f;
        }
        else if (randomValue > 0.9999f)
        {
            randomValue = 0.9999f;
        }

        triggerIntervalSamples_ = -std::log(randomValue) / density_ * sampleRate_;
    }
    else
    {
        triggerIntervalSamples_ = static_cast<float>(sampleRate_) / density_;
    }
}

void GrainScheduler::processTriggers(CircularBuffer& circularBuffer)
{
    if (sampleCounter_ >= triggerIntervalSamples_)
    {
        for (auto& grain : grainPool)
        {
            if (!grain.isActive())
            {
                GrainActivationParams params;

                float distributionOffset = distributionSamples_ * random_.nextFloat();
                distributionOffset = juce::jmax(0.0f, distributionOffset);
                params.startPosition = static_cast<float>(circularBuffer.getWritePosition()) -
                                       grainDurationSamples_ - distributionOffset;
                params.duration = applyVariation(random_, grainDurationSamples_, grainDurationVar_,
                                                 DistributionType::Gaussian);
                params.duration = juce::jmax(0.1f, params.duration);
                params.playbackRate = applyVariation(
                    random_, grainPlaybackRate_, grainPlaybackRateVar_, DistributionType::Gaussian);
                params.playbackRate = juce::jmax(0.1f, params.playbackRate);
                params.amplitude = grainAmplitude_;
                params.envelope = currentEnvelope_;

                grain.activate(params);

                break;
            }
        }
        calculateTriggerIntervalSamples();
        sampleCounter_ = 0;
    }

    sampleCounter_++;
}

float GrainScheduler::sumGrainsForChannel(CircularBuffer& circularBuffer, int channel)
{
    float output = 0.f;
    int activeCount = 0;

    for (auto& grain : grainPool)
    {
        if (grain.isActive())
        {
            output += grain.getSampleForChannel(circularBuffer, channel);
            ++activeCount;
        }
    }

    if (activeCount > 0)
        output /= std::sqrt(static_cast<float>(activeCount));

    return output;
}

void GrainScheduler::advanceGrains()
{
    for (auto& grain : grainPool)
    {
        if (grain.isActive())
            grain.advance();
    }
}
