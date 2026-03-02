//
//  CircularBuffer.cpp
//  MicoForge
//
//  Created by Ben on 10.09.25.
//

// #include <cmath>
#include "CircularBuffer.h"

CircularBuffer::CircularBuffer()
    : totalNumOutputChannels(0)
{
    // Empty - will be properly initialized in prepareToPlay
}

CircularBuffer::CircularBuffer(int numChannels, int circBufferSize)
    : totalNumOutputChannels(numChannels)
{
    circularBuffer_.setSize(totalNumOutputChannels, circBufferSize);
}

void CircularBuffer::fillBuffer(juce::AudioBuffer<float>& buffer, int channel)
{
    if (totalNumOutputChannels == 0)
        return; // Not initialized yet

    auto bufferSize = buffer.getNumSamples();
    auto circularBufferSize = circularBuffer_.getNumSamples();

    if (circularBufferSize == 0)
        return; // Safety check

    // Check to see if main buffer can be copied to circular buffer without needing to wrap
    if (circularBufferSize > bufferSize + writePosition_)
    {
        // Copy main buffer content to circular buffer
        circularBuffer_.copyFrom(channel, writePosition_, buffer.getWritePointer(channel),
                                 bufferSize);
    }
    else
    {
        // Check how much space is left at the end of the circular buffer
        auto numSamplesToEnd = static_cast<int>(circularBufferSize - writePosition_);

        // Copy that amount to the end
        circularBuffer_.copyFrom(channel, writePosition_, buffer.getWritePointer(channel),
                                 numSamplesToEnd);

        // Calculate how much content is remaining to copy
        auto numSamplesAtStart = static_cast<int>(bufferSize - numSamplesToEnd);

        // Bounds check before accessing buffer
        if (numSamplesToEnd >= bufferSize || numSamplesAtStart <= 0)
            return;

        // Copy remaining amount to beginning of circular buffer
        circularBuffer_.copyFrom(channel, 0, buffer.getWritePointer(channel, numSamplesToEnd),
                                 numSamplesAtStart);
    }
}

void CircularBuffer::readFromBuffer(juce::AudioBuffer<float>& buffer, int channel, float timeValue,
                                    float gainValue, double sampleRate)
{
    if (totalNumOutputChannels == 0)
        return; // Not initialized yet

    auto bufferSize = buffer.getNumSamples();
    auto circularBufferSize = circularBuffer_.getNumSamples();

    if (circularBufferSize == 0)
        return; // Safety check
    // auto t = apvts.getRawParameterValue("TIME")->load();
    auto t = timeValue;

    // 1 second of audio from in the past in circular buffer
    auto readPosition = static_cast<int>(writePosition_ - (sampleRate * t));

    if (readPosition < 0)
    {
        readPosition += circularBufferSize;
    }

    // auto g = 0.7f;
    // auto g = apvts.getRawParameterValue("GAIN")->load();
    auto g = gainValue;

    //
    if (readPosition + bufferSize < circularBufferSize)
    {
        buffer.addFromWithRamp(channel, 0, circularBuffer_.getReadPointer(channel, readPosition),
                               bufferSize, g, g);
    }
    else
    {
        auto numSamplesToEnd = static_cast<int>(circularBufferSize - readPosition);
        buffer.addFromWithRamp(channel, 0, circularBuffer_.getReadPointer(channel, readPosition),
                               numSamplesToEnd, g, g);

        auto numSamplesAtStart = static_cast<int>(bufferSize - numSamplesToEnd);
        buffer.addFromWithRamp(channel, numSamplesToEnd, circularBuffer_.getReadPointer(channel, 0),
                               numSamplesAtStart, g, g);
    }
}

void CircularBuffer::updateBufferPositions(juce::AudioBuffer<float>& buffer)
{
    if (totalNumOutputChannels == 0)
        return; // Not initialized yet

    auto bufferSize = buffer.getNumSamples();
    auto circularBufferSize = circularBuffer_.getNumSamples();

    if (circularBufferSize == 0)
        return; // Safety check

    writePosition_ += bufferSize;

    writePosition_ %= circularBufferSize;
}

float CircularBuffer::getSample(int channel, float position) const
{
    float sample = 0.0f;

    auto circularBufferSize = circularBuffer_.getNumSamples();
    // position < 0 ?
    while (position < 0)
    {
        position += circularBufferSize;
    }
    // position >= bufferSize ?
    while (position >= circularBufferSize)
    {
        position -= circularBufferSize;
    }

    // fractional positions 1000.5
    float fractionalPositionRemainder = position - floor(position);

    // keep branching for now
    // test performance difference to always interpolating later
    if (fractionalPositionRemainder > 0.0f)
    {
        int index1 = static_cast<int>(floor(position));
        int index2 = (index1 + 1) % circularBufferSize;

        float sampleIndex1 = circularBuffer_.getSample(channel, index1);
        float sampleIndex2 = circularBuffer_.getSample(channel, index2);

        float result = static_cast<float>(sampleIndex1) * (1.0f - fractionalPositionRemainder) +
                       static_cast<float>(sampleIndex2) * fractionalPositionRemainder;

        sample = result;
    }
    else
    {
        sample = circularBuffer_.getSample(channel, static_cast<int>(position));
    }

    return sample;
}

int CircularBuffer::getWritePosition() { return writePosition_; }

void CircularBuffer::writeSample(int channel, float sample)
{
    auto circularBufferSize = circularBuffer_.getNumSamples();
    if (circularBufferSize == 0)
    {
        return;
    }
    circularBuffer_.setSample(channel, writePosition_, sample);
}

void CircularBuffer::advanceWritePosition()
{
    auto circularBufferSize = circularBuffer_.getNumSamples();
    if (circularBufferSize == 0)
    {
        return;
    }

    writePosition_ = (writePosition_ + 1) % circularBufferSize;
}
