//
//  CircularBuffer.h
//  MicoForge
//
//  Created by Ben on 10.09.25.
//

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <stdio.h>

/**
 * Lock-free circular audio buffer for real-time granular processing.
 *
 * Incoming audio is written continuously into circularBuffer_ by fillBuffer().
 * Grains read back from arbitrary positions via getSample(), which wraps the
 * position modulo the buffer length and performs linear interpolation between
 * neighbouring samples.
 *
 * The delay buffer in PluginProcessor uses the lower-level writeSample() /
 * advanceWritePosition() / getSample() interface for sample-accurate feedback.
 *
 * Copy construction is disabled; move semantics are supported so that
 * prepareToPlay() can replace the buffer instance safely.
 */
class CircularBuffer
{
public:
    int totalNumOutputChannels;

    CircularBuffer();

    /**
     * Constructs a buffer with the given number of channels and size.
     * @param totalNumOutputChannels Number of audio channels
     * @param circularBufferSize     Buffer length in samples
     */
    CircularBuffer(int totalNumOutputChannels, int circularBufferSize);

    CircularBuffer(const CircularBuffer&) = delete;
    CircularBuffer& operator=(const CircularBuffer&) = delete;
    CircularBuffer(CircularBuffer&&) = default;
    CircularBuffer& operator=(CircularBuffer&&) = default;

    /**
     * Copies one channel of the audio block into the circular buffer.
     * Called at the start of processBlock for each channel.
     * @param buffer  The current audio block
     * @param channel Channel index to fill from
     */
    void fillBuffer(juce::AudioBuffer<float>& buffer, int channel);

    /**
     * Reads a delayed signal from the buffer back into the audio block.
     * Used for simple delay-line playback without granular processing.
     * @param buffer     Destination audio block
     * @param channel    Channel index
     * @param timeValue  Delay time in seconds
     * @param gainValue  Output gain
     * @param sampleRate Host sample rate in Hz
     */
    void readFromBuffer(juce::AudioBuffer<float>& buffer, int channel, float timeValue,
                        float gainValue, double sampleRate);

    /**
     * Advances writePosition_ by the number of samples in the block.
     * Must be called once at the end of processBlock after fillBuffer.
     * @param buffer The current audio block (used for block size)
     */
    void updateBufferPositions(juce::AudioBuffer<float>& buffer);

    /**
     * Reads a single sample at a fractional position using linear interpolation.
     * The position wraps around the buffer length automatically.
     * @param channel  Channel index
     * @param position Read position in samples (may be fractional and negative)
     * @return Interpolated sample value
     */
    float getSample(int channel, float position) const;

    /**
     * Returns the current write position (next sample to be written).
     * @return Write position in samples
     */
    int getWritePosition();

    /**
     * Writes a single sample to the current write position for one channel.
     * Does not advance the write position — call advanceWritePosition() separately.
     * @param channel Channel index
     * @param sample  Sample value to write
     */
    void writeSample(int channel, float sample);

    /**
     * Advances the write position by one sample, wrapping at buffer end.
     * Must be called once per sample after all channels have been written.
     */
    void advanceWritePosition();

private:
    juce::AudioBuffer<float> circularBuffer_;
    int writePosition_{0};
};
