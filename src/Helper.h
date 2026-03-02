//
//  Helper.h
//  MicoForge
//
//  Created by Ben on 22.01.26.
//

#pragma once

#include <cmath>
#include <juce_core/juce_core.h>
#include <random>

namespace HelperFunctions
{

/**
 * Distribution used by applyVariation() when scattering grain parameters.
 * Gaussian produces a bell-curve distribution centred on the base value;
 * Uniform distributes randomly across the full variation range.
 */
enum class DistributionType
{
    Gaussian,
    Uniform
};

/**
 * Generates a Gaussian-distributed random number using std::normal_distribution.
 *
 * @return A random value from a Gaussian distribution (mean=0, stdDev=1)
 */
inline float generateGaussian()
{
    static std::mt19937 generator{std::random_device{}()};
    static std::normal_distribution<float> distribution{0.0f, 1.0f};
    return distribution(generator);
}

/**
 * Applies a random variation to a parameter value.
 *
 * @param random A reference to a JUCE Random instance
 * @param paramValue The base parameter value
 * @param variationAmount The variation amount (0.0 to 1.0 for 0% to 100%)
 * @param distributionType The type of random distribution (Gaussian, Uniform)
 * @return The varied parameter value
 */
inline float applyVariation(juce::Random& random, float baseParameterValue, float variationAmount,
                            DistributionType distributionType)
{
    float variedParameterValue = 0.f;
    switch (distributionType)
    {
        case DistributionType::Gaussian:
            variedParameterValue =
                baseParameterValue * (1.f + variationAmount * generateGaussian());
            break;

        case DistributionType::Uniform:
            variedParameterValue =
                baseParameterValue * (1.f + variationAmount * random.nextFloat());
            break;

        default:
            variedParameterValue =
                baseParameterValue * (1.f + variationAmount * random.nextFloat());
            break;
    }
    return variedParameterValue;
}
} // namespace HelperFunctions
