//
//  Envelope.h
//  MicoForge
//
//  Created by Ben on 05.11.25.
//

#pragma once
#include <cmath>

/**
 * Stateless Envelope Interface
 *
 */
class GrainEnvelope
{
public:
    virtual ~GrainEnvelope() = default;

    /**
     * Calculates envelope value for normalized position
     * @param normalizedPosition Value within range of  0.0 (Start) und 1.0 (End)
     * @return Amplitude (within range 0.0 und 1.0)
     */
    virtual float processEnvelope(float normalizedPosition) const = 0;

    /**
     * Checks if position is within valid range
     * @param normalizedPosition Position to check
     * @return true if position is valid  (envelope is active)
     */
    virtual bool isValid(float normalizedPosition) const;
};

/**
 * Gaussian bell-curve envelope based on Roads (2002), p. 87.
 * Produces a smooth amplitude peak at the grain centre.
 * sigma controls the width of the bell relative to the grain duration.
 */
class GaussianEnvelope : public GrainEnvelope
{
public:
    GaussianEnvelope();

    /**
     * Evaluates the Gaussian bell curve at the given position.
     * @param normalizedPosition Normalised position within the grain (0.0–1.0)
     * @return Amplitude value (0.0–1.0)
     */
    float processEnvelope(float normalizedPosition) const override;
};

/**
 * Parabolic envelope (raised cosine approximation).
 * Defined by 4·t·(1-t), which equals zero at grain boundaries
 * and peaks at 1.0 in the centre — artifact-free and low cost.
 */
class ParabolicEnvelope : public GrainEnvelope
{
public:
    ParabolicEnvelope();

    /**
     * Evaluates the parabolic curve at the given position.
     * @param normalizedPosition Normalised position within the grain (0.0–1.0)
     * @return Amplitude value (0.0–1.0)
     */
    float processEnvelope(float normalizedPosition) const override;
};

// class QuasiGaussian : public GrainEnvelope {
// public:
//
// };
//
// class ThreeStageLineSeg {
// public:
//
// };
//
// class Triangular {
// public:
//
// };
//
// class SineFunc {
// public:
//
// };
//
// class Expodec {
// public:
//
// };
//
// class Rexpodec {
// public:
//
// };
