//
//  Grain.cpp
//  MicoForge
//
//  Created by Ben on 07.11.25.
//

#include "Grain.h"

Grain::Grain()
    : readPosition_(0.0f)
    , playbackRate_(1.0f)
    , envelopePosition_(0.0f)
    , totalDuration_(0.0f)
    , amplitude_(0.0f)
    , envelope_(nullptr)
    , isActive_(false)
{
}

void Grain::activate(const GrainActivationParams& params)
{
    if (params.duration <= 0.0f)
    {
        isActive_ = false;
        return;
    }
    else if (params.envelope == nullptr)
    {
        isActive_ = false;
        return;
    }

    readPosition_ = params.startPosition;
    playbackRate_ = params.playbackRate;
    envelopePosition_ = 0.0f;
    totalDuration_ = params.duration;
    amplitude_ = params.amplitude;
    envelope_ = params.envelope;
    isActive_ = true;
}

bool Grain::isActive() { return isActive_; }

float Grain::getCurrentReadPosition() { return readPosition_; }

float Grain::getSampleForChannel(CircularBuffer& circularBuffer, int channel)
{
    if (!isActive_)
    {
        return 0.0f;
    }

    float normalizedPosition = envelopePosition_ / totalDuration_;

    float envelopeAmplitude = envelope_->processEnvelope(normalizedPosition);

    float inputSample = circularBuffer.getSample(channel, readPosition_);

    float outputSample = inputSample * envelopeAmplitude * amplitude_;

    return outputSample;
}

void Grain::advance()
{
    envelopePosition_ += 1.0f;

    readPosition_ += playbackRate_;

    if (envelopePosition_ >= totalDuration_)
    {
        isActive_ = false;
    }
}
