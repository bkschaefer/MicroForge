//
//  Grain.h
//  MicoForge
//
//  Created by Ben on 07.11.25.
//

#pragma once

#include "../CircularBuffer.h"
#include "GrainEnvelope.h"
#include <stdio.h>

/**
 * Parameters passed to Grain::activate() by the GrainScheduler.
 * All positional values are in samples unless stated otherwise.
 */
struct GrainActivationParams
{
    float startPosition;           // read position in the circular buffer (samples)
    float duration;                // grain duration in samples
    float playbackRate;            // playback speed multiplier (1.0 = normal)
    float amplitude;               // output gain, range 0.0 - 1.0
    const GrainEnvelope* envelope; // pointer to envelope (shared, stateless)
};

/**
 * Single grain in the granular processor.
 *
 * A Grain reads a short audio fragment from a CircularBuffer, shapes it
 * with a GrainEnvelope, and advances its read position each sample.
 * Grains are managed in a pool by GrainScheduler and reused to avoid
 * allocation in the audio thread.
 */
class Grain
{
public:
    Grain();

    /**
     * Activates the grain with the given parameters.
     * Sets isActive_ to false and returns early if duration is zero
     * or the envelope pointer is null.
     * @param params Activation parameters provided by the scheduler
     */
    void activate(const GrainActivationParams& params);

    /**
     * Returns whether the grain is currently producing output.
     * @return true while the grain has not yet reached its end
     */
    bool isActive();

    /**
     * Returns the current read position within the circular buffer.
     * @return Read position in samples (fractional)
     */
    float getCurrentReadPosition();

    /**
     * Reads and shapes one output sample for the given channel.
     * Applies the envelope and amplitude to the raw sample from the buffer.
     * @param circularBuffer Source buffer to read from
     * @param channel        Channel index
     * @return Processed output sample
     */
    float getSampleForChannel(CircularBuffer& circularBuffer, int channel);

    /**
     * Advances the grain by one sample.
     * Increments the envelope position and the read position by playbackRate_.
     * Deactivates the grain when the envelope position reaches totalDuration_.
     */
    void advance();

private:
    float readPosition_;            // current read position in the circular buffer
    float playbackRate_;            // samples advanced per call to advance()
    float envelopePosition_;        // elapsed samples since activation
    float totalDuration_;           // grain length in samples
    float amplitude_;               // output gain
    const GrainEnvelope* envelope_; // non-owning pointer to the active envelope
    bool isActive_;
};
