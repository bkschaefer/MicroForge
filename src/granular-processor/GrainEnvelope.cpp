//
//  Envelope.cpp
//  MicoForge
//
//  Created by Ben on 06.11.25.
//

#include "GrainEnvelope.h"
#include <stdio.h>

bool GrainEnvelope::isValid(float normalizedPosition) const
{
    return normalizedPosition >= 0.0f && normalizedPosition <= 1.0f;
}

GaussianEnvelope::GaussianEnvelope() {}

float GaussianEnvelope::processEnvelope(float normalizedPosition) const
{
    constexpr float sigma = 0.15f;
    return exp(-0.5f * pow((normalizedPosition - 0.5f) / (sigma * 0.5f), 2.f));
}

ParabolicEnvelope::ParabolicEnvelope() {}

float ParabolicEnvelope::processEnvelope(float normalizedPosition) const
{
    return 4.0f * normalizedPosition * (1.0f - normalizedPosition);
}
